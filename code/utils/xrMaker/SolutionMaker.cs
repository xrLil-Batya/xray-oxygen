using System.IO;

namespace xrMaker
{
	class CMakeSolution
	{
		string RefPath;
		public CMakeSolution(string RefDir)
		{
			RefPath = RefDir;
		}

		private void MakeCSProject(string BinaryPath)
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
				"  <Import Project=\"$(MSBuildToolsPath)\\Microsoft.CSharp.targets\" />",
				"</Project>"
			};

			foreach (var Str in Header)
			{
				byte[] array = System.Text.Encoding.Default.GetBytes(Str + "\n");
				fsStream.Write(array, 0, array.Length);
			}
			fsStream.Close();
		}

		public void Make(string BinaryPath)
		{
			MakeCSProject(BinaryPath);
			FileStream fsStream = new FileStream(RefPath + @"\xrSpectre.sln", FileMode.Create);

			// Print header
			//string VBProjectName = "xrExternalDotScripts.csproj";
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
				//"Project(\"{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}\") = \"xrExternalDotScripts\", ",
				//$"\"{VBProjectName}\",",
				//" \"{A0DF394C-760C-497B-B325-0AAA1E7EF1FD}\"\n",
				//"EndProject\n",
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
				byte[] array = System.Text.Encoding.Default.GetBytes(Str);
				fsStream.Write(array, 0, array.Length);
			}
			fsStream.Close();
		}
	}
}
