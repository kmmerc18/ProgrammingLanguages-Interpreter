Class Main inherits IO {
    main(): Object {
        {
        let x:Int <- 7, t:Int in {
            let y:Int<-x in { 
                x<-(y-2); 
                out_int(x); 
                y; 
            };
        };
        let m: Int <- 7 + (let x:Int<-7 in { let y:Int<-x in { x<-(y-2); out_int(x); y; }; } ) in {
            out_int(m);
        };
        self.out_string("THIS IS A DISPATCH");
        let m: Int <- 7 + (let x:Int<-7 in { let y:Int<-x in { x<-(y-2); out_int(x); y; }; } ) in {
            out_int(m);
        };
        let m: Int <- 7 + (let x:Int<-7 in { let y:Int<-x in { x<-(y-2); out_int(x); y; }; } ) in {
            out_int(m);
        };
        let m: Int <- 7 + (let x:Int<-7 in { let y:Int<-x in { x<-(y-2); out_int(x); y; }; } ) in {
            out_int(m);
        };

        let z: Something <- (new Something).init(10, 20), w : SomethingELSE, b : Something in {
            w <- (new SomethingELSE).init(z,z);
            b <- z;
            out_int(b.getX());
            out_int(b.getY());
            out_int(w.getXX());
            out_int(w.getXY());
            out_int(w.getYY());
            out_int(w.getYX());
            out_int(z.getX());
            out_int(z.getY());
            z.changeX(12);
            z.changeY(24);
            out_int(w.getXX());
            out_int(w.getXY());
            out_int(w.getYY());
            out_int(w.getYX());
            out_int(z.getX());
            out_int(z.getY());
            out_int(b.getX());
            out_int(b.getY());
            };
        }
    };
};

Class Something {
    x : Int;
    y : Int;

    init(m:Int, v:Int) : Something {
        {
        x <- m;
        y <- v;
        self;
        }
    };

    changeX(t: Int): Something {
        {
        x <- t;
        self;
        }
    };

    getX(): Int {
        x
    };
    getY(): Int {
        y
    };

    changeY(t: Int) : Something {
        {
        y <- t;
        self;
        }
    };
};

Class SomethingELSE {
    x : Something;
    y : Something;

    init(m:Something, v:Something) : SomethingELSE{
        {
        x <- m;
        y <- v;
        self;
        }
    };

    getXX() : Int {
        x.getX()
    };
    getYY() : Int{
        y.getY()
    };
    getXY() : Int {
        x.getY()
    };
    getYX() : Int {
        y.getX()
    };
};