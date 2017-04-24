import com.sun.tools.attach.*;

import java.io.IOException;
import java.util.List;

/**
 * Created by Todd on 4/23/2017.
 */
public class Inection {

    public static void main(String[] args) throws IOException, AttachNotSupportedException, AgentLoadException, AgentInitializationException {
        String name = "Exilent Launcher.jar";
        List<VirtualMachineDescriptor> vms = VirtualMachine.list();
        String pid = "15576";

        //VirtualMachine vm = VirtualMachine.attach(pid);
        for (VirtualMachineDescriptor vmd : vms) {
            System.out.println(vmd.displayName());
            //System.out.println(vmd.id());
            if (vmd.displayName().equals(name)) {
                VirtualMachine vm = VirtualMachine.attach(vmd.id());
                System.out.println("Found it!");
                String agent = "C:\\Users\\Todd\\Documents\\Visual Studio 2015\\Projects\\ConsoleApplication1\\x64\\Release\\ConsoleApplication1.dll";
                vm.loadAgentPath(agent);
            }
        }
    }
}
//E:\downloads\Downloads\boost_1_60_0\boost_1_60_0
//  E:\downloads\Downloads\boost_1_60_0\boost_1_60_0\stage\lib



