using System;
using System.IO;
using Sharpmake;

[Generate]
public class Renderer : CommonProject
{
    public Renderer() { }

    public override void ConfigureAll(Configuration conf, EngineTarget target)
    {
        base.ConfigureAll(conf, target);

        conf.AddPublicDependency<Engine>(target);
        conf.AddPublicDependency<RenderCore>(target);
        conf.AddPublicDependency<RHI>(target);
    }
}