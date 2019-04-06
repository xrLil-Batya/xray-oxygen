using System.IO;
using System.Collections.Generic;

namespace xrMaker
{
	class CMakeSolution
	{
		string RefPath;
		string BinaryPath;
		public CMakeSolution(string RefDir)
		{
			RefPath = RefDir;

			// Make output dir
			if(!Directory.Exists(RefPath))
				Directory.CreateDirectory(RefPath);
		}
		private void MakeVBProject()
		{
			FileStream fsStream = new FileStream(RefPath + @"\xrExternalDotScripts.vbproj", FileMode.Create);
			string[] Header =
			{
				"<?xml version=\"1.0\" encoding=\"utf-8\"?> \r\n",
				"<Project ToolsVersion=\"15.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\"> \r\n",
				"  <Import Project=\"$(MSBuildExtensionsPath)\\$(MSBuildToolsVersion)\\Microsoft.Common.props\" Condition=\"Exists('$(MSBuildExtensionsPath)\\$(MSBuildToolsVersion)\\Microsoft.Common.props')\" /> \r\n",
				"  <PropertyGroup> \r\n",
				"    <Configuration Condition=\" '$(Configuration)' == '' \">Debug</Configuration> \r\n",
				"    <Platform Condition=\" '$(Platform)' == '' \">AnyCPU</Platform> \r\n",
				"    <ProjectGuid>{9BBE10D4-DB93-45F7-A013-64FCB32AD4B2}</ProjectGuid> \r\n",
				"    <OutputType>Library</OutputType> \r\n",
				"    <RootNamespace>xrExternalDotScripts</RootNamespace> \r\n",
				"    <AssemblyName>xrExternalDotScripts</AssemblyName> \r\n",
				"    <FileAlignment>512</FileAlignment> \r\n",
				"    <MyType>Windows</MyType> \r\n",
				"    <TargetFrameworkVersion>v4.6.1</TargetFrameworkVersion> \r\n",
				"    <Deterministic>true</Deterministic> \r\n",
				"  </PropertyGroup> \r\n",
				"  <PropertyGroup Condition=\" '$(Configuration)|$(Platform)' == 'Debug|AnyCPU' \"> \r\n",
				"    <DebugSymbols>true</DebugSymbols> \r\n",
				"    <DebugType>full</DebugType> \r\n",
				"    <DefineDebug>true</DefineDebug> \r\n",
				"    <DefineTrace>true</DefineTrace> \r\n",
				"    <OutputPath>bin\\Debug\\</OutputPath> \r\n",
				"    <DocumentationFile>xrExternalDotScripts.xml</DocumentationFile> \r\n",
				"    <NoWarn>42016,41999,42017,42018,42019,42032,42036,42020,42021,42022</NoWarn> \r\n",
				"  </PropertyGroup> \r\n",
				"  <PropertyGroup Condition=\" '$(Configuration)|$(Platform)' == 'Release|AnyCPU' \"> \r\n",
				"    <DebugType>pdbonly</DebugType> \r\n",
				"    <DefineDebug>false</DefineDebug> \r\n",
				"    <DefineTrace>true</DefineTrace> \r\n",
				"    <Optimize>true</Optimize> \r\n",
				"    <OutputPath>bin\\Release\\</OutputPath> \r\n",
				"    <DocumentationFile>xrExternalDotScripts.xml</DocumentationFile> \r\n",
				"    <NoWarn>42016,41999,42017,42018,42019,42032,42036,42020,42021,42022</NoWarn> \r\n",
				"  </PropertyGroup> \r\n",
				"  <PropertyGroup> \r\n",
				"    <OptionExplicit>On</OptionExplicit> \r\n",
				"  </PropertyGroup> \r\n",
				"  <PropertyGroup> \r\n",
				"    <OptionCompare>Binary</OptionCompare> \r\n",
				"  </PropertyGroup> \r\n",
				"  <PropertyGroup> \r\n",
				"    <OptionStrict>Off</OptionStrict> \r\n",
				"  </PropertyGroup> \r\n",
				"  <PropertyGroup> \r\n",
				"    <OptionInfer>On</OptionInfer> \r\n",
				"  </PropertyGroup> \r\n",
				"  <ItemGroup> \r\n",
				"    <Reference Include=\"System\" /> \r\n",
				"    <Reference Include=\"System.Data\" /> \r\n",
				"    <Reference Include=\"System.Xml\" /> \r\n",
				"    <Reference Include=\"System.Core\" /> \r\n",
				"    <Reference Include=\"System.Xml.Linq\" /> \r\n",
				"    <Reference Include=\"System.Data.DataSetExtensions\" /> \r\n",
				"    <Reference Include=\"System.Net.Http\" /> \r\n",
				"    <Reference Include=\"xrManagedCoreLib\">",
				$"      <HintPath>{BinaryPath+ '\\'}xrManagedCoreLib.dll</HintPath>",
				"    </Reference>",
				"    <Reference Include=\"xrManagedEngineLib\">",
				$"      <HintPath>{BinaryPath+ '\\'}xrManagedEngineLib.dll</HintPath>",
				"    </Reference>",
				"    <Reference Include=\"xrManagedGameLib\">",
				$"      <HintPath>{BinaryPath+ '\\'}xrManagedGameLib.dll</HintPath>",
				"    </Reference>",
				"    <Reference Include=\"xrManagedRenderLib\">",
				$"      <HintPath>{BinaryPath+ '\\'}xrManagedRenderLib.dll</HintPath>",
				"    </Reference>",
				"    <Reference Include=\"xrManagedUILib\">",
				$"      <HintPath>{BinaryPath + '\\'}xrManagedUILib.dll</HintPath>",
				"    </Reference>",
				"  </ItemGroup> \r\n",
				"  <ItemGroup> \r\n",
				"    <Import Include=\"Microsoft.VisualBasic\" /> \r\n",
				"    <Import Include=\"System\" /> \r\n",
				"    <Import Include=\"System.Collections\" /> \r\n",
				"    <Import Include=\"System.Collections.Generic\" /> \r\n",
				"    <Import Include=\"System.Data\" /> \r\n",
				"    <Import Include=\"System.Diagnostics\" /> \r\n",
				"    <Import Include=\"System.Linq\" /> \r\n",
				"    <Import Include=\"System.Xml.Linq\" /> \r\n",
				"    <Import Include=\"System.Threading.Tasks\" /> \r\n",
				"  </ItemGroup> \r\n",
				"  <Import Project=\"$(MSBuildToolsPath)\\Microsoft.VisualBasic.targets\" /> \r\n",
				"  <ItemGroup>\r\n",
				"	<ProjectReference Include=\"xrDotScripts.csproj\"> \r\n",
				"		<Project>{03709499-bc33-44ee-9bae-7e4adb921a3a}</Project> \r\n",
				"			<Name>xrDotScripts</Name> \r\n",
				"		</ProjectReference> \r\n",
				"  </ItemGroup> \r\n"
			};

			foreach (var Str in Header)
			{
				byte[] sArray = System.Text.Encoding.UTF8.GetBytes(Str + "\n");
				fsStream.Write(sArray, 0, sArray.Length);
			}

			List<string> CompilerFilesList = new List<string>
			{
				"<ItemGroup> \r\n"
			};

			AddScript(ref CompilerFilesList, "vb");

			CompilerFilesList.Add("</ItemGroup> \r\n");
			CompilerFilesList.Add("</Project> \r\n");

			foreach (var FileName in CompilerFilesList)
			{
				byte[] array = System.Text.Encoding.UTF8.GetBytes(FileName);
				fsStream.Write(array, 0, array.Length);
			}
			fsStream.Close();
		}
		private void AddScript(ref List<string> StrList, string Ext)
		{
			string ScriptsPath = BinaryPath + @"\..\gamedata\scripts";
			if (Directory.Exists(ScriptsPath))
			{
				string[] FileList = Directory.GetFiles(ScriptsPath, $"*.{Ext}", SearchOption.AllDirectories);

				foreach (var FileName in FileList)
					StrList.Add($"<Compile Include=\"{FileName}\" /> \r\n");
			}
		}
		private void MakeCSProject()
		{
			FileStream fsStream = new FileStream(RefPath + @"\xrDotScripts.csproj", FileMode.Create);
			string[] Header =
			{
				"<?xml version=\"1.0\" encoding=\"utf-8\"?>",
				"<Project ToolsVersion=\"15.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">",
				"  <Import Project=\"$(MSBuildExtensionsPath)\\$(MSBuildToolsVersion)\\Microsoft.Common.props\" Condition=\"Exists('$(MSBuildExtensionsPath)\\$(MSBuildToolsVersion)\\Microsoft.Common.props')\" />",
				"  <PropertyGroup>",
				"    <Configuration Condition=\" '$(Configuration)' == '' \">Debug</Configuration>",
				"    <Platform Condition=\" '$(Platform)' == '' \">AnyCPU</Platform>",
				"    <ProjectGuid>{03709499-BC33-44EE-9BAE-7E4ADB921A3A}</ProjectGuid>",
				"    <OutputType>Library</OutputType>",
				"    <AppDesignerFolder>Properties</AppDesignerFolder>",
				"    <RootNamespace>SlnTest</RootNamespace>",
				"    <AssemblyName>SlnTest</AssemblyName>",
				"    <TargetFrameworkVersion>v4.6.1</TargetFrameworkVersion>",
				"    <FileAlignment>512</FileAlignment>",
				"    <Deterministic>true</Deterministic>",
				"  </PropertyGroup>",
				"  <PropertyGroup Condition=\" '$(Configuration)|$(Platform)' == 'Debug|AnyCPU' \">",
				"    <DebugSymbols>true</DebugSymbols>",
				"    <DebugType>full</DebugType>",
				"    <Optimize>false</Optimize>",
				@"    <OutputPath>bin\Debug\</OutputPath>",
				"    <DefineConstants>DEBUG;TRACE</DefineConstants>",
				"    <ErrorReport>prompt</ErrorReport>",
				"    <WarningLevel>4</WarningLevel>",
				"  </PropertyGroup>",
				"  <PropertyGroup Condition=\" '$(Configuration)|$(Platform)' == 'Release|AnyCPU' \">",
				"    <DebugType>pdbonly</DebugType>",
				"    <Optimize>true</Optimize>",
				@"    <OutputPath>bin\Release\</OutputPath>",
				"    <DefineConstants>TRACE</DefineConstants>",
				"    <ErrorReport>prompt</ErrorReport>",
				"    <WarningLevel>4</WarningLevel>",
				"  </PropertyGroup>",
				"  <ItemGroup>",
				"    <Reference Include=\"System\" />",
				"    <Reference Include=\"System.Core\" />",
				"    <Reference Include=\"System.Xml.Linq\" />",
				"    <Reference Include=\"System.Data.DataSetExtensions\" />",
				"    <Reference Include=\"Microsoft.CSharp\" />",
				"    <Reference Include=\"System.Data\" />",
				"    <Reference Include=\"System.Net.Http\" />",
				"    <Reference Include=\"System.Xml\" />",
				"    <Reference Include=\"xrManagedCoreLib\">",
				$"      <HintPath>{BinaryPath+ '\\'}xrManagedCoreLib.dll</HintPath>",
				"    </Reference>",
				"    <Reference Include=\"xrManagedEngineLib\">",
				$"      <HintPath>{BinaryPath+ '\\'}xrManagedEngineLib.dll</HintPath>",
				"    </Reference>",
				"    <Reference Include=\"xrManagedGameLib\">",
				$"      <HintPath>{BinaryPath+ '\\'}xrManagedGameLib.dll</HintPath>",
				"    </Reference>",
				"    <Reference Include=\"xrManagedRenderLib\">",
				$"      <HintPath>{BinaryPath+ '\\'}xrManagedRenderLib.dll</HintPath>",
				"    </Reference>",
				"    <Reference Include=\"xrManagedUILib\">",
				$"      <HintPath>{BinaryPath + '\\'}xrManagedUILib.dll</HintPath>",
				"    </Reference>",
				"  </ItemGroup>",
				"  <Import Project=\"$(MSBuildToolsPath)\\Microsoft.CSharp.targets\" />"
			};

			foreach (var Str in Header)
			{
				byte[] sArray = System.Text.Encoding.UTF8.GetBytes(Str + "\n");
				fsStream.Write(sArray, 0, sArray.Length);
			}

			List<string> CompilerFilesList = new List<string>
			{
				"<ItemGroup> \r\n"
			};

			AddScript(ref CompilerFilesList, "cs");

			CompilerFilesList.Add("</ItemGroup> \r\n");
			CompilerFilesList.Add("</Project> \r\n");

			foreach (var FileName in CompilerFilesList)
			{
				byte[] array = System.Text.Encoding.UTF8.GetBytes(FileName);
				fsStream.Write(array, 0, array.Length);
			}
			fsStream.Close();
		}
		public void Make(string sBinaryPath)
		{
			BinaryPath = sBinaryPath;
			MakeCSProject();
			MakeVBProject();
			FileStream fsStream = new FileStream(RefPath + @"\xrSpectre.sln", FileMode.Create);

			// Print header
			string VBProjectName = "xrExternalDotScripts.vbproj";
			string CSProjectName = "xrDotScripts.csproj";
			string[] Header =
				{
				"Microsoft Visual Studio Solution File, Format Version 12.00\n",
				"# Visual Studio 15\n",
				"VisualStudioVersion = 15.0.28010.2036\n",
				"MinimumVisualStudioVersion = 10.0.40219.1\n",
				"Project(\"{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}\") = \"xrDotScripts\", ",
				$"\"{CSProjectName}\",",
				"\"{03709499-BC33-44EE-9BAE-7E4ADB921A3A}\"\n",
				"EndProject\n",
				"\n",
				"Project(\"{F184B08F-C81C-45F6-A57F-5ABD9991F28F}\") = \"xrExternalDotScripts\", ",
				$"\"{VBProjectName}\",",
				" \"{A0DF394C-760C-497B-B325-0AAA1E7EF1FD}\"\n",
				"EndProject\n",
				"Global\n",
				"	GlobalSection(SolutionConfigurationPlatforms) = preSolution\n",
				"		Debug|Any CPU = Debug|Any CPU\n",
				"		Release|Any CPU = Release|Any CPU\n",
				"	EndGlobalSection\n",
				"	GlobalSection(ProjectConfigurationPlatforms) = postSolution\n",
				"		{03709499-BC33-44EE-9BAE-7E4ADB921A3A}.Debug|Any CPU.ActiveCfg = Debug|Any CPU\n",
				"		{03709499-BC33-44EE-9BAE-7E4ADB921A3A}.Debug|Any CPU.Build.0 = Debug|Any CPU\n",
				"		{03709499-BC33-44EE-9BAE-7E4ADB921A3A}.Release|Any CPU.ActiveCfg = Release|Any CPU\n",
				"		{03709499-BC33-44EE-9BAE-7E4ADB921A3A}.Release|Any CPU.Build.0 = Release|Any CPU\n",
				"		{A0DF394C-760C-497B-B325-0AAA1E7EF1FD}.Debug|Any CPU.ActiveCfg = Debug|Any CPU\n",
				"		{A0DF394C-760C-497B-B325-0AAA1E7EF1FD}.Debug|Any CPU.Build.0 = Debug|Any CPU\n",
				"		{A0DF394C-760C-497B-B325-0AAA1E7EF1FD}.Release|Any CPU.ActiveCfg = Release|Any CPU\n",
				"		{A0DF394C-760C-497B-B325-0AAA1E7EF1FD}.Release|Any CPU.Build.0 = Release|Any CPU\n",
				"	EndGlobalSection\n",
				"	GlobalSection(SolutionProperties) = preSolution\n",
				"		HideSolutionNode = FALSE\n",
				"	EndGlobalSection\n",
				"	GlobalSection(ExtensibilityGlobals) = postSolution\n",
				"		SolutionGuid = {66734DEC-E7B9-4437-893E-40FBD77394AA}\n",
				"	EndGlobalSection\n",
				"EndGlobal\n"
				};

			foreach (var Str in Header)
			{
				byte[] array = System.Text.Encoding.UTF8.GetBytes(Str);
				fsStream.Write(array, 0, array.Length);
			}
			fsStream.Close();
		}
	}
}
