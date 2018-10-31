#include "stdafx.h"
#include "ScriptCompiler.h"
#include "API/Log.h"

using namespace System::Reflection;
using namespace System::IO;
using namespace System::CodeDom::Compiler;
using namespace Microsoft;

xrScriptCompiler::xrScriptCompiler()
{
	PrivateInfo = new xrScriptCompiler_Private();
	CSSourceCodes = gcnew List<String ^>();
	VBSourceCodes = gcnew List<String ^>();
	Parameters = gcnew CompilerParameters();
}

xrScriptCompiler::~xrScriptCompiler()
{
	delete PrivateInfo;
}

CompilerResults^ xrScriptCompiler::FindCSScripts()
{
	string_path ScriptFolder;
	FS.update_path(ScriptFolder, "$game_scripts$", "");

	FS_FileSet CSScriptFiles;
	FS.file_list(CSScriptFiles, ScriptFolder, FS_ListFiles, "*.cs");

	if (CSScriptFiles.empty())
	{
		Log("Spectre compiler: nothing compile, skip");
		return nullptr;
	}

	for (const FS_File& file : CSScriptFiles)
	{
		string_path scriptPath;
		FS.update_path(scriptPath, "$game_scripts$", file.name.c_str());

		CSSourceCodes->Add(gcnew String(scriptPath));
	}

	CSharp::CSharpCodeProvider^ provider = gcnew CSharp::CSharpCodeProvider();
	return provider->CompileAssemblyFromFile(Parameters, CSSourceCodes->ToArray());
}

CompilerResults^ xrScriptCompiler::FindVBScripts()
{
	string_path ScriptFolder;
	FS.update_path(ScriptFolder, "$game_scripts$", "");

	FS_FileSet VBScriptFiles;
	FS.file_list(VBScriptFiles, ScriptFolder, FS_ListFiles, "*.vb");

	for (const FS_File& file : VBScriptFiles)
	{
		string_path scriptPath;
		FS.update_path(scriptPath, "$game_scripts$", file.name.c_str());

		VBSourceCodes->Add(gcnew String(scriptPath));
	}

	VisualBasic::VBCodeProvider^ provider = gcnew VisualBasic::VBCodeProvider();
	return provider->CompileAssemblyFromFile(Parameters, VBSourceCodes->ToArray());
}

bool xrScriptCompiler::CompileScripts()
{
	Parameters->ReferencedAssemblies->Add(GetPathToThisAssembly());
	Parameters->ReferencedAssemblies->Add(GetPathToBuildAssembly("xrManagedEngineLib.dll"));
	Parameters->ReferencedAssemblies->Add(GetPathToBuildAssembly("xrManagedGameLib.dll"));
	Parameters->ReferencedAssemblies->Add(GetPathToBuildAssembly("xrManagedUILib.dll"));
	Parameters->GenerateInMemory = false;
	Parameters->GenerateExecutable = false;

	///#TODO: Generate release version without debug info
	Parameters->IncludeDebugInformation = true;
	Parameters->OutputAssembly = "xrDotScripts.dll";
	//list all files in directory
	Parameters->CompilerOptions = "-platform:x64";

	CompilerResults^ result = FindCSScripts();
	if (!result)
	{
		result = FindVBScripts();
		if (!result)
			return false;
	}

	if (result->Errors->HasErrors)
	{
		//print errors
		Log("! CSharp compile failed!");
		System::Text::StringBuilder^ sb = gcnew System::Text::StringBuilder();
		for (int errInd = 0; errInd < result->Errors->Count; errInd++)
		{
			CompilerError^ error = result->Errors[errInd];
			sb->Append(error->ToString());
			sb->Append(L" \n");
		}
		String^ ErrLog = sb->ToString();
		XRay::Log::Error(ErrLog);
		return false;
	}

	//check for warnings
	if (result->Errors->HasWarnings)
	{
		Log("@ CSharp compile have warnings");
		System::Text::StringBuilder^ sb = gcnew System::Text::StringBuilder();
		for (int errInd = 0; errInd < result->Errors->Count; errInd++)
		{
			CompilerError^ error = result->Errors[errInd];
			sb->Append('!');
			sb->Append(error->ToString());
			sb->Append(" \r\n");
		}
		String^ ErrLog = sb->ToString();
		XRay::Log::Error(ErrLog);
	}

	//load assembly
	Assembly^ ScriptModule = nullptr;
	String^ ModuleAddress = System::IO::Directory::GetCurrentDirectory() + "\\" + Parameters->OutputAssembly;
	try
	{
		ScriptModule = Assembly::LoadFile(ModuleAddress);
	}
	catch (FileNotFoundException^ fileNotFound)
	{
		Log("! CSharp compile can't load compiled file. Can't write to disk (Not enough space?)");
		XRay::Log::Error(fileNotFound->FusionLog);
		return false;
	}
	catch (FileLoadException^ FileLoadExp)
	{
		Log("! CSharp compile can't load compiled file. Incorrect file format");
		XRay::Log::Error(FileLoadExp->FusionLog);
		return false;
	}
	catch (ArgumentNullException^)
	{
		Log("! CSharp compile can't compile source code");
		return false;
	}
	catch (BadImageFormatException^ invalidImageExp)
	{
		Log("! CSharp compile can't load compiled file. Incorrect file format");
		XRay::Log::Error(invalidImageExp->FusionLog);
		return false;
	}

	if (ScriptModule == nullptr)
	{
		Log("! CSharp compile can't compile source code");
		return false;
	}

	scriptAssembly = ScriptModule;
	if (scriptAssembly == nullptr) return false;
	//invoke in all classes 'OnLoad'

	return true;
}

System::Reflection::Assembly^ xrScriptCompiler::GetAssembly()
{
	return scriptAssembly;
}

System::String^ xrScriptCompiler::GetPathToThisAssembly()
{
	String^ codeBase = Assembly::GetExecutingAssembly()->CodeBase;
	UriBuilder^ uri = gcnew UriBuilder(codeBase);
	String^ path = Uri::UnescapeDataString(uri->Path);
	return path;
}

System::String^ xrScriptCompiler::GetPathToBuildAssembly(String^ assemblyName)
{
	String^ BinFileName = Assembly::GetCallingAssembly()->Location;
	String^ BinDirectory = System::IO::Path::GetDirectoryName(BinFileName);
	String^ LoadFilename = BinDirectory + "\\" + assemblyName;
	return LoadFilename;
}
