(* hello-world.cl *)
class Main inherits IO {
  main() : Object {
    {
      let m : Hello in
        m.out_int(7);

      let x: Int <- in_int() in {
        out_int(x);
        out_string("\n");
        let y: Int <- in_int() in {
          out_int(in_int());
          out_string("\n");
        };
      };
    }
  };
} ;

class Hello inherits IO {
};