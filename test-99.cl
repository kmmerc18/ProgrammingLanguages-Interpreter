class Main inherits IO {
    main(): Object{
        {

            let this_test: Test <- (new Test).init(0) in

            out_string(this_test.test(in_string()));
        }
    };
};

class Test inherits Main {

    num: Int;

    init(x: Int): Test {
        {
            num <- x;
            self;
        }
    };

    -- make 1000 Cool Activation Records
    test(a: String): String {
        {
            out_string(a);
            num <- a.length() + num;
            out_int(num);
            a.concat(test_2(a));
        }
    };

    test_2(a: String): String {
        {
            out_string(a);
            num <- a.length() + num;
            out_int(num);
            a.concat(test(a));
        }
    };
};