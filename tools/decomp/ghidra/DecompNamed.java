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
        for (int i = 1; i < args.length; i++) {
            String name = args[i];
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
            try (FileWriter w = new FileWriter(new File(out, name + ".c"))) {
                w.write(text);
            }
            println("OK " + name + " -> " + fn.getName(true));
        }
        ifc.dispose();
    }
}
