#include "stdafx.h"
#include "ScriptCompiler.h"
#include "API/Log.h"

using namespace System::Reflection;
using namespace System::IO;
using namespace System::CodeDom::Compiler;
using namespace Microsoft::CSharp;

xrScriptCompiler::xrScriptCompiler()
{
	PrivateInfo = new xrScriptCompiler_Private();
	SourceCodes = gcnew List<String ^>();
}

xrScriptCompiler::~xrScriptCompiler()
{
	delete PrivateInfo;
}

bool xrScriptCompiler::CompileScripts()
{
	string_path ScriptFolder;
	string_path ShaderFolder;
	FS.update_path(ScriptFolder, "$game_scripts$", "");
	FS.update_path(ShaderFolder, "$game_shaders$", "");

	FS_FileSet ScriptFiles;
	FS_FileSet ShaderScriptFiles;
	FS.file_list(ScriptFiles, ScriptFolder, FS_ListFiles, "*.cs");
	//FS.file_list(ShaderScriptFiles, ShaderFolder, FS_ListFiles, "*.cs");

	if (ScriptFiles.empty() && ShaderScriptFiles.empty())
	{
		Log("CSharp compiler: nothing compile, skip");
		return false;
	}

	for (const FS_File& file : ShaderScriptFiles)
	{
		string_path scriptPath;
		FS.update_path(scriptPath, "$game_shaders$", file.name.c_str());

		String^ sourceFile = gcnew String(scriptPath);
		SourceCodes->Add(sourceFile);
	}

	for (const FS_File& file : ScriptFiles)
	{
		string_path scriptPath;
		FS.update_path(scriptPath, "$game_scripts$", file.name.c_str());

		String^ sourceFile = gcnew String(scriptPath);
		SourceCodes->Add(sourceFile);
	}

	CSharpCodeProvider^ provider = gcnew CSharpCodeProvider();
	CompilerParameters^ parameters = gcnew CompilerParameters();
	parameters->ReferencedAssemblies->Add(GetPathToThisAssembly());
	parameters->ReferencedAssemblies->Add(GetPathToBuildAssembly("xrManagedEngineLib.dll"));
	parameters->ReferencedAssemblies->Add(GetPathToBuildAssembly("xrManagedGameLib.dll"));
	parameters->ReferencedAssemblies->Add(GetPathToBuildAssembly("xrManagedUILib.dll"));
	parameters->GenerateInMemory = false;
	parameters->GenerateExecutable = false;
	///#TODO: Generate release version without debug info
	parameters->IncludeDebugInformation = true;
	parameters->OutputAssembly = "xrDotScripts.dll";
	//list all files in directory
	parameters->CompilerOptions = "-platform:x64";
	CompilerResults^ result = provider->CompileAssemblyFromFile(parameters, SourceCodes->ToArray());
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
	String^ ModuleAddress = System::IO::Directory::GetCurrentDirectory() + "\\" + parameters->OutputAssembly;
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
