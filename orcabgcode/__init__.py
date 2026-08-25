try:
    import orca
except ImportError:
    orca = None

import pybgcode

if orca is not None:
    class BGCodeExporterCapability(orca.slicing.SlicingPipelineCapabilityBase):
        def get_name(self):
            return "Export to BGCode"

        def execute(self, ctx):
            if ctx.step == orca.slicing.Step.psGCodePostProcess:
                input_path = ctx.gcode_path
                output_path = input_path.replace(".gcode", ".bgcode")
                
                # Use libbgcode bindings to convert
                infile = pybgcode.open(input_path, "r")
                if not pybgcode.is_open(infile):
                    return orca.ExecutionResult.failure(
                        orca.PluginResult.FatalError,
                        "Failed to open input G-code file"
                    )
                
                # Wait, pybgcode.open allows "wb" or "w"?
                # From pybgcode bindings: m.def("open", [](const char * name, const char *mode) { FILE * fptr = boost::nowide::fopen(name, mode); ...
                outfile = pybgcode.open(output_path, "wb")
                if not pybgcode.is_open(outfile):
                    pybgcode.close(infile)
                    return orca.ExecutionResult.failure(
                        orca.PluginResult.FatalError,
                        "Failed to open output BG-code file"
                    )
                
                config = pybgcode.get_config()
                res = pybgcode.from_ascii_to_binary(infile, outfile, config)
                
                pybgcode.close(infile)
                pybgcode.close(outfile)
                
                if res == pybgcode.EResult.Success:
                    return orca.ExecutionResult.success("Exported to BGCode")
                else:
                    return orca.ExecutionResult.failure(
                        orca.PluginResult.FatalError, 
                        f"Failed to export to BGCode (Code: {res})"
                    )
            return orca.ExecutionResult.skipped()

    @orca.plugin
    class BGCodePlugin(orca.base):
        def register_capabilities(self):
            orca.register_capability(BGCodeExporterCapability)
