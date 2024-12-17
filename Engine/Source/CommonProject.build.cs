using System;
using System.Collections;
using Microsoft.Win32;
using Sharpmake;

[module:Include("Utils.cs")]

public abstract class CommonProject : Project
{
    protected CommonProject(bool bAddTarget = true) : base(typeof(EngineTarget))
    {
        Name = GetType().Name;

        IsFileNameToLower = false;
        IsTargetFileNameToLower = false;

        SourceRootPath = @"[project.RootPath]";

        // .cs 파일이 실제로 빌드에 포함되지는 않지만, 프로젝트에 추가되어 수정에 용이하게 함
        SourceFilesExtensions.Add(".cs");
        //SourceFilesCompileExtensions.Add(".ixx");

        if (bAddTarget == true)
        {
            AddTargets(new EngineTarget(
                    ELaunchType.Editor | ELaunchType.Client | ELaunchType.Server,
                    Platform.win64,
                    DevEnv.vs2022,
                    Optimization.Debug | Optimization.Release
            ));
        }
    }

    [Configure]
    public virtual void ConfigureAll(Configuration conf, EngineTarget target)
    {
        Utils.MakeConfiturationNameDefine(conf, target);
        conf.DumpDependencyGraph = true;

        // conf.Output = Configuration.OutputType.Exe;
        if (target.LaunchType == ELaunchType.Editor)
        {
            conf.Output = Configuration.OutputType.Dll;
        }
        else
        {
            conf.Output = Configuration.OutputType.Lib;
        }
        conf.Options.Add(Options.Vc.General.CharacterSet.Unicode);
        conf.Options.Add(Options.Vc.Compiler.JumboBuild.Enable);
        conf.MinFilesPerJumboFile = 100;
        conf.Options.Add(Options.Vc.Compiler.CppLanguageStandard.CPP20);
        //conf.Options.Add(Options.Vc.Compiler.CppLanguageStandard.Latest);
        if (target.Optimization == Optimization.Debug)
        {
            conf.Options.Add(Options.Vc.Compiler.Inline.Default);

            // Visual Studio 핫리로드 대응
            // UE Live Coding과 유사
            conf.Options.Add(Options.Vc.Linker.Incremental.Enable);
            conf.Options.Add(Options.Vc.General.DebugInformation.ProgramDatabaseEnC);
            conf.Options.Add(Options.Vc.Compiler.FunctionLevelLinking.Enable);
        }

        // RTTI
        // UE에서는 꺼져있지만, dynamic_cast를 쓰기 위해 추가
        conf.Options.Add(Options.Vc.Compiler.RTTI.Enable);

        conf.ProjectFileName = "[project.Name]";

        // Exceptions
        conf.Options.Add(Options.Vc.Compiler.Exceptions.Enable);

        // Debug
        {
            conf.VcxprojUserFile = new Configuration.VcxprojUserFileSettings();
            conf.VcxprojUserFile.LocalDebuggerWorkingDirectory = "$(OutputPath)";
        }

        // Path
        {
            conf.ProjectPath = Utils.GetProjectDir();

            string SolutionDir = Utils.GetSolutionDir();
            conf.TargetPath = SolutionDir + @"/Binaries/" + conf.Name;
            conf.IntermediatePath = SolutionDir + @"/Intermediate/Build/" + conf.Name + "/[project.Name]/";

            // Include
            {
                //conf.IncludePrivatePaths.Add("[project.SourceRootPath]");
                conf.IncludePrivatePaths.Add(conf.ProjectPath);

                string HeaderParserTargetDir = SolutionDir + @"/Intermediate/HeaderParser/HeaderParserGenerated/[project.Name]";
                conf.IncludePaths.Add(HeaderParserTargetDir);
                conf.IncludePaths.Add(@"[project.SourceRootPath]");
                conf.IncludePaths.Add(@"[project.SourceRootPath]\Public");
            }
        }

        // UE Module 규칙
        // UE의 경우 모든 모듈이 IMPLEMENT_MODULE_모듈이름의 매크로가 정의되어 있어야함 
        // 이 이름으로 DLL에서 함수를 로드해서 호출함
        if (target.LaunchType == ELaunchType.Editor)
        {
            conf.ForceSymbolReferences.Add("IMPLEMENT_MODULE_" + conf.Project.Name);
        }

        // Runtime Library
        {
            //if (target.LaunchType == ELaunchType.Editor)
            {
                if (target.Optimization == Optimization.Debug)
                    conf.Options.Add(Options.Vc.Compiler.RuntimeLibrary.MultiThreadedDebugDLL);
                else
                    conf.Options.Add(Options.Vc.Compiler.RuntimeLibrary.MultiThreadedDLL);
            }
            //else
            //{
            //    if (target.Optimization == Optimization.Debug)
            //        conf.Options.Add(Options.Vc.Compiler.RuntimeLibrary.MultiThreadedDebug);
            //    else
            //        conf.Options.Add(Options.Vc.Compiler.RuntimeLibrary.MultiThreaded);
            //}
        }

        // 빌드 직전에 HeaderParser가 실행되면서 변경된 헤더들을 수집하고, generated.h를 생성
        string EngineDir = Utils.GetEngineDir();
        conf.EventPreBuild.Add(@"cmd /c """"" + EngineDir + @"\Engine\Source\Programs\HeaderParser\HeaderParser.bat"" ""$(SolutionDir)"" [project.Name] ""[project.SourceRootPath]"" " + @"""" + EngineDir + @"""""");

        conf.CustomProperties.Add("CustomOptimizationProperty", $"Custom-{target.Optimization}");
    }
}