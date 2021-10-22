-- Adapted from the CRM

class Silly {
    var1 : Int <- 0;
    var2 : Int <- 0;
    set_var1(v : Int) : SELF_TYPE {
        {
        var1 <- v;
        self;
        }
    };
    set_var2(v : Int) : SELF_TYPE {
        {
        var2 <- v;
        self;
        }
    };
    get_var1() : Int {
        var1
    };
    get_var2() : Int {
        var2
    };
};

class Sally inherits Silly { };

class Main inherits IO {
	x : Silly <- (new Silly).copy();
    y : Sally <- new Sally;
    z : Silly;
    a : Sally;

	main() : Sally {{ 
        z <- y.copy();

        out_int(x.get_var1());
        out_int(x.get_var2());
        out_int(y.get_var1());
        out_int(y.get_var2());
        out_int(z.get_var1());
        out_int(z.get_var2());

        y.set_var1(5);
        z.set_var2(2);

        a <- y.copy();
        a.set_var1(1);
        a.set_var2(19);

        out_int(a.get_var1());
        out_int(a.get_var2());
        out_int(y.get_var1());
        out_int(y.get_var2());
        out_int(z.get_var1());
        out_int(z.get_var2());

        y.set_var1(23);
        y.set_var2(4);
        z.set_var1(8);
        z.set_var2(3);

        out_int(a.get_var1());
        out_int(a.get_var2());
        out_int(y.get_var1());
        out_int(y.get_var2());
        out_int(z.get_var1());
        out_int(z.get_var2());

        out_string(x.type_name());
        out_string(y.type_name());
        out_string(z.type_name());
        out_string(a.type_name());

        a;
    }};
};