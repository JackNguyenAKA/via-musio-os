package com.akaintelligence.musio.library.system;

import android.util.Log;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

/**
 * Created by james on 16. 11. 8.
 */

public class Toolbox {
  public static boolean command(String cmd) {
    if ("error".equals(exec(cmd))) {
      return false;
    } else {
      return true;
    }
  }

  public static String exec(String cmd) {
    Log.d("COMMAND", "$ " + cmd);
    try {
      Process p = Runtime.getRuntime().exec(new String[]{"sh", "-c", cmd});
      BufferedReader br = new BufferedReader(new InputStreamReader(p.getInputStream()));
      String result = "";
      String line;
      while (!((line = br.readLine()) == null)) {
        Log.d("COMMAND", "  " + line);
        result += line + "\n";
      }
      br.close();
      p.getInputStream().close();
      int lastIndex = result.lastIndexOf("\n");
      if(lastIndex!=-1) result = result.substring(0,lastIndex);
      return result;
    } catch (IOException e) {
      return "error";
    }
  }
}
