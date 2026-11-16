// Headless post-script: decompile functions named by their mangled symbol.
// Args: <output dir> <mangled name> [<mangled name> ...]
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileOptions;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.Symbol;
import ghidra.program.model.symbol.SymbolIterator;

import java.io.File;
import java.io.FileWriter;

public class DecompNamed extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        File out = new File(args[0]);
        out.mkdirs();
        DecompInterface ifc = new DecompInterface();
        ifc.setOptions(new DecompileOptions());
        ifc.openProgram(currentProgram);
        // "@<file>" reads the names from a UTF-8 file, one per line: template
        // names such as f<int, int> cannot pass through analyzeHeadless.bat
        // (cmd reads < and > as redirections) nor serve as file names, so each
        // result is written as fn<N>.c, N counting the names from 1.
        java.util.List<String> names = new java.util.ArrayList<>();
        for (int i = 1; i < args.length; i++) {
            if (args[i].startsWith("@")) {
                for (String l : java.nio.file.Files.readAllLines(new File(args[i].substring(1)).toPath(),
                                                                  java.nio.charset.StandardCharsets.UTF_8)) {
                    if (!l.trim().isEmpty()) names.add(l.trim());
                }
            } else {
                names.add(args[i]);
            }
        }
        for (int i = 1; i <= names.size(); i++) {
            String name = names.get(i - 1);
            Function fn = null;
            SymbolIterator it = currentProgram.getSymbolTable().getSymbols(name);
            while (it.hasNext() && fn == null) {
                Symbol s = it.next();
                fn = getFunctionAt(s.getAddress());
            }
            if (fn == null) {
                // the demangler may have replaced the label; match on any symbol at a function entry
                for (Function f : currentProgram.getFunctionManager().getFunctions(true)) {
                    for (Symbol s : currentProgram.getSymbolTable().getSymbols(f.getEntryPoint())) {
                        if (s.getName().equals(name)) { fn = f; break; }
                    }
                    if (fn != null) break;
                }
            }
            if (fn == null) {
                println("NOT FOUND " + name);
                continue;
            }
            DecompileResults r = ifc.decompileFunction(fn, 120, monitor);
            String text = r.decompileCompleted() ? r.getDecompiledFunction().getC() : ("// failed: " + r.getErrorMessage());
            try (FileWriter w = new FileWriter(new File(out, "fn" + i + ".c"))) {
                w.write(text);
            }
            println("OK " + name + " -> " + fn.getName(true));
        }
        ifc.dispose();
    }
}
