myparts = $$split(QMAKESPEC,"/")
OS = $$take_last(myparts)
$$take_last(myparts)
COMPILER = $$take_last(myparts)
contains(COMPILER, qt5){
    COMPILER = gcc_64
    OS = $$split(OS,"-")
    OS = $$member(OS,0)
}else{
contains(COMPILER, ios){
    os_temp = $$OS
    OS = $$COMPILER
    COMPILER = $$os_temp

}else{

    contains(COMPILER, mkspecs){
        COMPILER = $$OS
    }

    contains(COMPILER, msvc2017){
        contains(COMPILER, msvc2017_64){
        }
        else
            COMPILER = msvc2017_32
    }
    OS = $$split(OS,"-")
    OS = $$member(OS,0)
}
}
