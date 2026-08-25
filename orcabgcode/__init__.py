import orca
import pybgcode


class BGCodePlugin(orca.slicing.SlicingPipelineCapabilityBase):
    def get_name(self):
        return "BGCode Support"

    def execute(self, ctx):
        if ctx.step != orca.slicing.Step.psGCodePostProcess:
            return orca.ExecutionResult.success()

        try:
            infile = pybgcode.open(input_path, "r")
        except Exception as exc:
            return orca.ExecutionResult.failure(
                orca.PluginResult.RecoverableError,
                f"No fue posible leer el archivo: {exc}",
            )

        if not pybgcode.is_open(infile):
            return orca.ExecutionResult.failure(
                orca.PluginResult.RecoverableError,
                "Failed to open input G-code file"
            )
        
        orca.host.ui.message("Export finished. Open the folder?",
            title="My Plugin", buttons="yes_no", icon="question")

        return orca.ExecutionResult.success("Exported to BGCode")


@orca.plugin
    class BGCodePlugin(orca.base):
        def register_capabilities(self):
            orca.register_capability(BGCodeExporterCapability)