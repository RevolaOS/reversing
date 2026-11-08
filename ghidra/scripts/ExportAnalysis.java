// SPDX-License-Identifier: GPL-3.0-only
// Adapted from RepriseOS ghidra/scripts/ExportAnalysis.java (8fc840d).
// Export the current program's analysis as SARIF without its memory bytes.
// Arguments: output directory, then the project paths to export.
// @category Revola

import ghidra.app.script.GhidraScript;
import ghidra.app.util.importer.MessageLog;
import java.io.File;
import java.util.Arrays;
import sarif.SarifProgramOptions;
import sarif.managers.ProgramSarifMgr;

public class ExportAnalysis extends GhidraScript {
  @Override
  public void run() throws Exception {
    String[] args = getScriptArgs();
    String path = currentProgram.getDomainFile().getPathname().substring(1);
    if (!Arrays.asList(args).subList(1, args.length).contains(path)) {
      println("Skipping unlisted program " + path);
      return;
    }
    File output = new File(args[0], path + ".sarif");
    output.getParentFile().mkdirs();
    SarifProgramOptions options = new SarifProgramOptions();
    options.setProperties(false);
    options.setTrees(false);
    new ProgramSarifMgr(currentProgram, output, new MessageLog())
        .write(currentProgram, currentProgram.getMemory(), monitor, options);
    // Firmware bytes are never published.
    new File(output + ".bytes").delete();
    println("Exported " + path);
  }
}
