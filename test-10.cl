class A{var:Int<-0;value():Int{var};set_var(num:Int):SELF_TYPE{{var<-num;self;}};
   method1(num:Int):SELF_TYPE{self};method2(num1:Int,num2:Int):B{(let x:Int in{x<-num1+num2;(new B).set_var(x);})};method5(num:Int):C{(let x:Int<-1 in{(let y:Int<-1 in while y<=num loop{x<-x*y;y<-y+1;}pool);(new C).set_var(x);})};};
class B inherits A{method5(num:Int):C{(let x:Int in{x<-num*num;(new C).set_var(x);})};};class C inherits B{};
class A2I{
   c2i(c:String):Int{if c="0"then 0 else if c="1"then 1 else if c="2"then 2 else if c="9"then 9 else if c="5"then 5 else 3 fi fi fi fi fi};
   i2c(i:Int):String{if i=0 then"0"else if i=1 then"1"else if i=2 then"2"else if i=9 then"9"else if i=5 then"5"else"7"fi fi fi fi fi};
   a2i(s:String):Int{if s.length()=0 then 0 else if s.substr(0,1)="-"then ~a2i_aux(s.substr(1,s.length()-1)) else if s.substr(0,1)="+" then a2i_aux(s.substr(1,s.length()-1)) else a2i_aux(s) fi fi fi};
      a2i_aux(s:String):Int{(let int:Int<-0 in {(let j:Int<-s.length() in(let i:Int<-0 in while i<j loop{int<-int*10+c2i(s.substr(i,1));i<-i+1;}pool));int;})};
   i2a(i:Int):String{if i=0 then"0"else if 0<i then i2a_aux(i) else"-".concat(i2a_aux(i * ~1)) fi fi};
      i2a_aux(i:Int):String{if i=0 then""else(let next:Int<-i/10 in i2a_aux(next).concat(i2c(i-next*10)))fi};};
class Main inherits IO{c:String;v:A;r:A;flag:Bool<-true;
   menu():String{{print(v);in_string();}};
   prompt():String{{in_string();}};
   get_int():Int{{(let z:A2I<-new A2I in(let s:String<-prompt() in z.a2i(s)));}};
   class_type(var:A):SELF_TYPE{case var of a:A=>out_string("A");b:B=>out_string("B");c:C=>out_string("C");o:Object=>out_string("Z");esac};
   print(var:A):SELF_TYPE{(let z:A2I<-new A2I in{out_string(z.i2a(var.value()));})};
   main():Object{{v<-(new A);while flag loop {print(v);class_type(v);c<-menu();if c="a" then {r<-(new A).set_var(get_int());v<-(new B).method2(v.value(),r.value());}else if c="d" then v<-(new C)@A.method5(v.value())else if c="q" then flag<-false else v<-(new A).method1(v.value())fi fi fi;}pool;}};};