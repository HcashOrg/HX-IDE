package gjavac.test.java;

import gjavac.lib.Contract;
import gjavac.lib.Offline;
import gjavac.lib.UvmContract;

import static gjavac.lib.UvmCoreLibs.*;

@Contract(storage = Storage.class)
class DemoContract extends UvmContract<Storage> {

    @Override
    public void init() {
        print("init of demo java Contract");
        this.getStorage().name = "my_contract";
        print("storage.name changed to " + this.getStorage().getName());
        pprint(this.getStorage());
    }

    public String sayHi(String name) {
        print("say hi api called of contract to " + name);
        return "Hello" + name;
    }

    public void setName(String name) {
        this.getStorage().setName(name);
        pprint("name changed to " + name);
    }

    public String getName(String arg) {
        print("storage.name=" + this.getStorage().getName());
        String name = this.getStorage().getName();
        return name;
    }

    @Offline
    public String offlineGetInfo(String arg) {
        print("offlineGetInfo called with arg " + arg);
        Utils utils = new Utils();
        print("3+4=" + utils.sum(3, 4));
        return "hello, offline api";
    }

}