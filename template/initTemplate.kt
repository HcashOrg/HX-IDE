package gjavac.test.kotlin

import gjavac.lib.*

class Storage {
    var name: String? = null
//    var age: Int? = null
}

@Component
class Utils {
    fun sum(a: Int, b: Int): Int {
        UvmCoreLibs.print("this is util sum func")
        return a + b
    }
}

@Contract(storage = Storage::class)
class MyContract : UvmContract<Storage>() {
    override fun init() {
        val a: Byte = 123
        UvmCoreLibs.print("init of demo Contract " + a)
        this.storage?.name = "my_contract" // TODO: getfield error in chain
        UvmCoreLibs.print("storage.name changed")
        UvmCoreLibs.pprint(this.storage)
        UvmCoreLibs.print(this.storage?.name)
//        this.storage?.age = 25 // FIXME: maybe crash
    }

    fun sayHi(name: String): String {
        UvmCoreLibs.print("say hi api called of contract to $name")
        return "Hello" + name
    }

    fun setName(name: String) {
        this.storage?.name = name
        UvmCoreLibs.pprint("name changed to " + name)
    }

    fun getName(arg: String) {
        UvmCoreLibs.print("storage.name=" + this.storage?.name)
    }

    @Offline
    fun offlineGetInfo(arg: String): String {
        UvmCoreLibs.print("offlineGetInfo called with arg " + arg)
//        testUtil() // FIXME: change to get this["testUtil"]()
        val utils = Utils()
        UvmCoreLibs.print("3+4=" + utils.sum(3, 4))
        return "hello, offline api"
    }

    private fun testUtil() {
        UvmCoreLibs.print("this is util func not in apis")
    }
}

class Person {

    // TODO: event, LCMP, LREM etc.

    fun sayHi(name: String): String {
        UvmCoreLibs.print("sayHi func called")
        return "Hello$name java. I love money"
    }

    companion object {
//
//        fun sum(a: String, b: String): String {
//            return a + b
//        }
    }

    fun testIf(): Boolean {
        val a = 1
        val b = 2
        val c = 3
        if(a<b) {
            UvmCoreLibs.print("$a < $b")
        }
        if(c>=b) {
            UvmCoreLibs.print("$c>=$b")
        }
        if(b>c) {
            UvmCoreLibs.print("error here")
        } else {
            UvmCoreLibs.print("$b <= $c")
        }
        return true
    }

    fun testFor() {
        UvmCoreLibs.print("testFor cases")
        var sum = 0
        // TODO
    }

    fun testWhile() {
        UvmCoreLibs.print("testWhile cases")
        var sum = 0
        var i = 0
        while(i<10) {
            sum += i
            i++
            println(i)
        }
        UvmCoreLibs.print("sum=$sum")
        var j = 100
        var sum2 = 0
        do {
            sum2 += j
            j--
        }while(j>=0)
        println("sum2=$sum2")
    }

//     fun testWhen() {
//         println("testWhen cases")
//         var i = 0
//         while(i<4) {
//             println(i)
//             when(i) {
//                 0 -> println("$i=0")
//                 1->println("$i=1")
//                 2->println("$i=2")
//                 else -> println("$i>2")
//             }
//             i++
//         }
//     }

    // TODO: test continue, break

    fun test_pprint() {
        UvmCoreLibs.pprint(this)
    }

    fun testNumber() {
        UvmCoreLibs.print("testNumber cases:")
        val a = 4
        val b = 5
        val c = 1.23
        val d = a + b
        val e = a + c
        val f = b*c
        val h = c/b
        UvmCoreLibs.print(c)
        println("$a + $b = $d")
        println("$a + $c = $e")
        println("$b * $c = $f")
        println("$c / $b = $h")

        println("1 = ${UvmCoreLibs.tointeger(c)}")
        println("~ ${a} = ${UvmCoreLibs.neg(a as Long)}")
    }

    fun testBooleans() {
        var a = true
        var b = false
        var c = a && b
        UvmCoreLibs.print("a && b=$c")
        println("a || b=${a||b}")
        if(a) {
            UvmCoreLibs.print("$a=true")
        }
        if(b) {
            UvmCoreLibs.print("error happend in boolean test")
        } else {
            UvmCoreLibs.print("$b=false")
        }
        var i=0
        while(i<3) {
            if(!(i>1)) {
                UvmCoreLibs.print("!($i>1)=true")
            }
            i++
        }
        UvmCoreLibs.print("!c=${!c}")
    }

    fun testArray() {
        UvmCoreLibs.print("testArray cases")
        val array1 = UvmArray.create<Int?>()
        array1.add(1)
        array1.add(2)
        array1.add(3)
        array1.add(4)
        UvmCoreLibs.pprint(array1)
        UvmCoreLibs.print("array 1 size is " + array1.size()) // 4
        array1.set(4, 2)
        UvmCoreLibs.print("array[4] is " + array1.get(4)) // 2
        array1.pop()
        UvmCoreLibs.print("array 1 after changed size is " + array1.size()) // 3
        UvmCoreLibs.print("array[3] is " + array1.get(3)) // 3
        UvmCoreLibs.pprint(array1)
        for (i in 1..(array1.size())) {
            val item = array1.get(i)
            UvmCoreLibs.print("index: " + i)
            UvmCoreLibs.print("value: " + item)
        }
        val array1Iter = array1.ipairs()
        var array1keyValuePair = array1Iter(array1, 0)
        while (array1keyValuePair.first != null)
        {
            UvmCoreLibs.print("key: " + array1keyValuePair.first)
            UvmCoreLibs.print("value: " + array1keyValuePair.second)
            array1keyValuePair = array1Iter(array1, array1keyValuePair.first)
        }
        array1.set(3, null)
        UvmCoreLibs.print("array1 size is ${array1.size()} after remove index 3")
        UvmCoreLibs.pprint(array1)
    }

    fun testMap() {
        val map1 = UvmMap.create<String>()
        map1.set("name", "C#")
        map1.set("country", "China")
        UvmCoreLibs.print("map1's name is " + map1.get("name"))
        UvmCoreLibs.print("map1's country is " + map1.get("country"))
        // 遍历map的demo
        val map1PairsIter = map1.pairs()
        UvmCoreLibs.pprint(map1PairsIter)
        var keyValuePair = map1PairsIter(map1, null)
        UvmCoreLibs.pprint(keyValuePair)
        UvmCoreLibs.print(keyValuePair.first)
        UvmCoreLibs.print(keyValuePair.second)
        while (keyValuePair.first != null)
        {
            UvmCoreLibs.print("key: " + UvmCoreLibs.tostring(keyValuePair.first))
            UvmCoreLibs.print("value: " + UvmCoreLibs.tostring(keyValuePair.second))
//            debug()
            if (keyValuePair.first == "name")
            {
                UvmCoreLibs.print("found key==name pair")
            }
            keyValuePair = map1PairsIter(map1, keyValuePair.first)
        }
        val table1 = map1 as UvmTable
        UvmCoreLibs.pprint("cast C#=" + (table1 as UvmMap<String>).get("name"))
    }

    fun testEvent() {
        UvmCoreLibs.emit("Hello", "World") // FIXME: change to emitEventMethod
    }

    fun testModules() {
        UvmCoreLibs.print("testModules cases")
        val strModule = UvmCoreLibs.importModule(UvmStringModule::class.java, "string")
        val len = strModule.len("Hello")
        UvmCoreLibs.print("Hello length is $len")

        var tableModule = UvmCoreLibs.importModule<UvmTableModule>(UvmTableModule::class.java, "table")
        var table1 = UvmArray.create<String>()
        table1.add("a")
        tableModule.append(table1, "b")
        var table1Count = tableModule.length(table1)
        UvmCoreLibs.print("table1 size is: " + table1Count)

        val mathModule = UvmCoreLibs.importModule<UvmMathModule>(UvmMathModule::class.java, "math")
        val floor1 = mathModule.floor(3.3)
        UvmCoreLibs.print("floor(3.3) = " + floor1)
        val abs1 = mathModule.abs(-4)
        UvmCoreLibs.print("abs(-4) = " + abs1)
        val pi = mathModule.pi
        UvmCoreLibs.print("pi = " + pi)
        val stoi1 = mathModule.tointeger("123")
        UvmCoreLibs.print("123=$stoi1")

        var timeModule = UvmCoreLibs.importModule<UvmTimeModule>(UvmTimeModule::class.java, "time");
        UvmCoreLibs.print("date: " + timeModule.tostr(1494301754))

        val jsonModule = UvmCoreLibs.importModule<UvmJsonModule>(UvmJsonModule::class.java, "json");
        UvmCoreLibs.print("dumps of json module is: " + jsonModule.dumps(jsonModule))
    }

    fun testImportContract() {
        // TODO
        val utilsContract = UvmCoreLibs.importContract(Utils::class.java, "utils")
        UvmCoreLibs.print("3+4=${utilsContract.sum(3, 4)}")
    }

    fun main(): MyContract {
        // entry point of contract
        UvmCoreLibs.print("hello uvm")
        val contract = MyContract()
        // don't init contract when compile to gpc, used as contract
//        if (contract is MyContract) {
//            print("contract is contract")
//            print(contract.sayHi("contract-name"))
//            contract.storage = Storage()
//            contract.init()
//            print("name="+contract.storage?.name)
//            val offlineApiRes = contract.offlineGetInfo("hi")
//            print("offline api res is $offlineApiRes")
//        }

        testIf()
        testNumber()
        testWhile()
//        testWhen()
        test_pprint()
        testBooleans()
        testArray()
        testMap()
        testEvent()
        testModules()
//        testImportContract()
        UvmCoreLibs.print(sayHi("hi-name"))
        return contract
    }
}