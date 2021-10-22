--- EXCEPTION division by 0 & newlines and tabs & default init (int 0, str "", bool false)
class Main inherits IO {
    int : Int;
    string : String;
    bool : Bool;

    main() : Int {{ 
        out_int(int);
        out_string(string);
        if isvoid string then out_string("void") else out_string("empty") fi;
        if bool then out_string("true") else out_string("false") fi;
        out_string("\t\t\n\n");
        9/0;
    }};
};