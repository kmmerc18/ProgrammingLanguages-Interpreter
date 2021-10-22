Class Main inherits General_Maths {

};
    
class General_Maths inherits IO {
    main(): Object { 
        let test: Int in {
            test <- in_int();
            out_int(test);
            test <- 0;
            test <- 67/test;
            out_int(test);
        }
    };
};