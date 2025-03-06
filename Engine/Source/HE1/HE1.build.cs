using System.IO;
using Sharpmake;

[Generate]
public class HE1 : CommonProject
{
    public HE1()
    {
        ResourceFilesExtensions.Add(".rc");
        ResourceFilesExtensions.Add(".ico");
        ResourceFilesExtensions.Add(".bmp");
        ResourceFilesExtensions.Add(".rc2");
    }
    public override void ConfigureAll(Configuration conf, EngineTarget target)
    {
        base.ConfigureAll(conf, target);
        if (target.LaunchType == ELaunchType.Editor)
        {
            conf.Options.Add(Options.Vc.General.MfcSupport.UseMfcDynamic);
            conf.Options.Add(Options.Vc.Linker.SubSystem.Windows);
        }

        //conf.SolutionFolder = "Engine";
        conf.Output = Configuration.OutputType.Exe;

        conf.AddPublicDependency<Core>(target);
        // Launch가 Core를 사용하고 있기 때문에, Launch만 포함해도 
        // 문제는 없지만, 명시적으로 Core를 추가시켜준다.
        conf.AddPublicDependency<Launch>(target);

        // if (target.LaunchType == ELaunchType.Editor || target.LaunchType == ELaunchType.Client)
        // {
        // conf.AddPublicDependency<Slate>(target);
        conf.AddPublicDependency<Renderer>(target);
        // conf.AddPublicDependency<D3D11RHI>(target);
        // }
    }
}
