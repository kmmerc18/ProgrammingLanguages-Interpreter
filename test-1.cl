(*
 * Here is a file with a dumb math class 
 *)
class Main inherits IO {
    main(): Object 
    {
        {
            out_string("Let's do some math!\n");
            
            -- here is a math with 3
            let math3: Math_with_X <- new Math_with_X in 
            { 
                math3 <- math3.init(3);     -- set x to 3
                let y: Int in
                { 
                    y <- math3.add_y(4); -- y = 3"4

                    let exp: Int in 
                    {
                        exp <- math3@Math_with_X.exp_y(y); -- exp = 3^7

                        out_string("3+4 is: ");
                        out_int(y);
                        out_string("\n3^7 is: ");
                        out_int(exp);
                        out_string("\n");
                    };

                    y <- 3;

                    -- imbed another y in these lets, dispatch on void, div by 0
                    let math2: Math_with_X <- new Math_with_X in {
                        math2 <- math2.init(2);

                        let y: Int, n: General_Math, z: Int <-1, s: String <-"hey" in 
                        {
                            n.add(2, y);
                            n <- new General_Math;
                            y <- math2.add_y(4); -- y = 2+4
                            z <- z/0;
                            s <- s.substr(4,10);
                             
                            let exp: Int in 
                            {
                                exp <- math2.exp_y(y); -- exp = 2^6

                                out_string("2+4 is: ");
                                out_int(y);
                                out_string("\n2^6 is: ");
                                out_int(exp);
                                out_string("\n");
                            };
                        };
                    };
                };
            };
        }
    };
};
-- there is a lot of curly braces there - jeez

class General_Math{

    add(x: Int, y: Int): Int{
        x + y
    };

    multiply(x: Int, y: Int): Int {
        x * y
    };

    exp(x: Int, y: Int): Int {
        let exp_result : Int <- 1 in {
            while 0 < y loop
            {
                exp_result <- exp_result*x;
                y <- y -1;
            } pool;
            exp_result;
        } 
    };
};

-- do math with a number x
class Math_with_X inherits General_Math
{
    this_x: Int;

    init(x: Int): Math_with_X 
    {
        {
            if 0 < x then
                this_x <- x
            else
                this_x <- 0
            fi;

            self;
        }
    };

    add_y(y: Int): Int { this_x + y};

    multiply_by_y(y: Int): Int
    {
        if y = 0 then 
            0
        else
            let result: Int in { 
                if 0 < y then
                    result <- add(this_x, multiply_by_y(y - 1)) 
                else result fi;
                result;
            }
        fi
    };

    exp_y(y: Int): Int
    {
        if y = 0 then 
            1
        else
            -- ooh recursion!
            let result: Int in { 
                if 0 < y then
                    result <- multiply(this_x, exp_y(y - 1)) 
                else result fi;
                result;
            }
        fi
    };
};