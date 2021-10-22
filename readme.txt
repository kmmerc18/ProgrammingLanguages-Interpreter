Haille Perkins, Corinna Pilcher, Kim Merchant 
Angstadt
16 April 2021

Overview of Teamwork---------------------------------------------------------------------------------------
	The work creating test cases to reveal bugs in SLUGs was relatively evenly split, as was work writing the readme. This team 
communicated effectively and made time for the project, meeting together throughout this project over Zoom frequently for long, 
productive sessions, while also working independently and keeping each other updated on our endeavors. The team would go to office 
hours as a group whenever possible. Our group was affected (like most groups, but we were more vocal about it we suspect) by the SLUGs 
bug that prevented our test cases from revealing some bugs, which extended the workload a bit. 
    The group took turns when typing in meetings, with some notable contributions outside of full-team meetings. 
Corinna completed the Let expression handling in interpreter.re and wrote the Copy expression. She also kept track of the cases 
we were not passing in SLUGs and what they involved to focus debugging efforts. Kim took notes on where we got stuck in team meetings 
and bug hunting, emailing updates and reminders of this information to the team along with notes taken regarding the issues and 
resolutions discussed in office hours.  She also worked a significant bit on IO.in_string, in which we had trouble handling null 
characters, as explained later in this document. Haille took the lead on ast1.re and is responsible for its completion outside of the 
provided code and team meetings. She also completed the New expression, comparisons, divide by zero, and math expression (featuring some 
lovely functional programming skills) handling in interpreter.re.  

Design Decisions: Ast1.re ----------------------------------------------------------------------------------
    The ast1.re file was created to read in a c.l-type file and create the class, interpretation, and parent maps that are required for 
various expression evaluations.  The specifications for Project 4 outlined the structure of the type file. The main components of the 
file are the read_cl_type_file function and the variant types for expressions, expression subparts, identifiers, class attributes, and 
entries for each of the previously mentioned maps.  The components of these custom types were determined by the P4 spec, as well.  
The first component of the file is the class_map section.  Here a list of classes is presented, each with a list of their attributes and 
whether or not these attributes are initialized.  Given there is a specific format to how this type of file outputs a list of elements, a 
read_list  function was created.  This function takes in an in_channel to read lines of the file and a function on how to read the elements 
of the list.  Since a list always begins with the number of elements, that line of the file is read in and used to ensure that only the 
specified number of elements are read in.  For each class in the class list, a list of attributes was also read in and saved in a 
class_map_entry.  This contains a string for the name of the class along with a list of attributes. The variant type for attributes, 
or attr, contain the attribute’s name, type, and associated initializing expression if needed.  Since attributes may have expressions 
linked with them, a read_exp function, exp type, exp_subparts variant type were developed. There is a subpart for each of the expression 
types available in COOL and each one contains all of the extra information about the expression listed, as described in the specification 
for Project 3. This structure benefitted from ReasonML’s ability to also recursive definitions for types. For example, an expression can 
have the If-statement subpart, which contains expressions within it. The two expression subparts that were less straightforward were Let 
and Case as they contain lists of specialized elements. This was quickly remedied with the recursive definitions allowed in ReasonML. The 
final class_map is a list of the previously mentioned class_map_entrys. 
    Next, the implementation map was read in.  This was similar to creating the class_map, just the elements within the larger list were 
imp_map_entrys.  These contain a string for the class name and list of methods for the given class. The method type is broken into two 
pieces: the method name and the associated formals, type and body.  The two-element tuples were made to make searching in the map with 
List.assoc easier later.  This built in Reason function is used in various locations in interpreter.re, so having the imp_map_entry and 
method types be two-element tuples allow the name of the class or method with List.assoc.  The result yields the desired information.  
Lastly, the parent_map is constructed.  This was the easiest of the three as each parent_map_entry was just a pair of strings, rather than 
the more complex type detailed above.	

Design Decisions: Interpreter.re---------------------------------------------------------------------------
	When the interpreter is run, first we create a global variable, activations, which is a counter beginning at zero and updated via 
the method update. Each time a dispatch of any type, or New, is called for, this variable will increase, and decrease upon that method’s 
completion. This way, the interpreter can track the number of activations, and if it ever reaches 1000 or more, will indicate a stack 
overflow in the interpreter.
	Next, the interpreter checks for the correct number of command line arguments passed in. If the number is not correct, meaning 
potentially that no cl-type-file has been passed in, then the interpreter prints a help message. Next, the interpreter opens the associated 
cl-type-file listed in the command line arguments. This file is then called the ast_file. The read_cl_type_file method from ast1.re takes 
the type file and returns a tuple of the class_map, imp_map, and parent_map  found within it if each of those is not malformed.
	After this, two types are created: location, which is an integer value representing the location information is stored at in the 
LocationMap (among other related uses) later on; and type value, which defines what basic type an expression is, including void, boolean, 
integer, string, or dynamic type. The type associated with a particular object can be accessed via the getType method. To create a type for 
the location map, a module OrderedLocation is created that allows locations to be in a map. The map of strings used as the environment is 
made with the String type rather than the OrderedLocation.
	The interpreter creates an empty LocationMap as the store (sto) which maps locations to values, an empty StringMap as the environment 
(env) that maps names to locations, and a self object (so) initialized to void because, until the interpreter reads the information offered 
through the ast, there may not be a self object in scope. Assuming that the program being interpreted contains a Main method, as it will 
because the program has passed the type-checking stage, the interpreter then creates a tuple init_exp that contains a 0 (representing the 
line number of the (new Main).main() call), the type name “object” (representing the resultant type of the main method, which always 
results in Object), and commits a dynamic dispatch to create the Object type Main with the method name main. It takes no arguments because 
COOL programs never take arguments in their Main declaration. Then, the interpreter begins to run through the expression in the ast, 
starting with this init_exp representing the main body of the interpreted program.
	The interpreter begins processing the information passed in from ast1.re through the method eval_expr, which takes in the current self 
object (so), the current environment (env) which is what is used to keep track of what identifiers and values are currently in scope, the 
current store (sto) used to track what each location maps to whether or not it is in scope, and the current expression expr being evaluated 
from the ast.
	Inside the eval_expr method, the interpreter decomposes the expression into its contents, which include the line number it was written 
on in the original COOL code (line), the type of the expression (typ_expr), and the expression itself (exp). Based on exp, the interpreter 
enters a switch statement. Different processes are applied depending on if exp qualifies as an Identifier, Assign, Dynamic Dispatch, Static 
Dispatch, Self Dispatch, If, While, Block, New, Isvoid, Plus, Minus, Times, Divide, Lt (less than), Eq (equal to), Le (less than or equal 
to), Not, Negate, Integer, String, boolean (True or False), Let, Case, or Internal statement.  These individual options in the switch 
statement are explained in more detail below, but essentially each element of an expression and its attributes are evaluated recursively 
by eval_expr until it reaches a basic type that requires no more evaluation, then each case returns the value the expression evaluated to, 
and the updated store sto that contains the side effects of evaluating the expression. This means that values are updated as they are 
changed by the effects of the COOL code.
	A number of helper functions make the processes in the interpreter easier. The method new_loc, which takes in the current store (sto) 
as a variable, and uses this to return the next available location in the LocationMap that does not contain a value. The recursive method 
fill_locs takes a list of the type we made earlier, location, as well as a list of values and the current store. Based on these, via a 
switch statement, the head value in the list of values is inserted into the head store of the list of locations, and then fill_locs is 
called recursively to fill the remaining locations with the remaining values. The next helper method, fill_sto_entry, in effect performs 
a similar task to fill_locs, except that it fills one individual location in the store with the value given, returning an updated store 
with the new value in it. The next method, fill_env_entry, does the same, filling a location with a name (rather than a value) in the 
environment map and returning the updated environment.
	Inside eval_expr there are a number of helper functions, labelled as “INTERNAL HELPERS”  in the comments. The first of these is the 
recursive dispatch_process, which accepts the arguments args passed into a dispatch expression, as well as the current store, and a list 
lst, which is passed in as empty in every use of this method, like an accumulator list. For each argument in args, the method uses 
eval_expr on the argument, getting the value v of the expression and an updated store (sto2). The value v of the expression is added to 
the accumulation list before the method recursively calls itself until there is no further expression in the args list.
	The next internal helper method for eval_expr is get_l, which takes a number n of locations to create, a list (lst) to add these 
locations to (once again, this list (lst) is always empty to start) and the current store (sto) to add this location information to. 
The method updates the store (sto) as the accumulator list (lst) grows. The method ends when there are n new, unused locations added to lst.
	The next helper method, get_default_vals, takes an accumulator list acc and a type (typ) and determines what default value should be 
added to the accumulator based on the (typ) passed in. For example, “String” would return the default value of a String with length 0 and 
value of an empty string, aka String(0,””).
	The last helper method within eval_expr is used to compute algebra. It is called compute_arithmetic, and it accepts two values, the 
operation to be applied between them, and a boolean describing whether or not the operation is division (in order to catch division-by-zero 
errors). Based on the values passed in, if they are both integers then the desired operation is carried out and the result and updated 
store containing it (sto3) are returned. If division occurred, as long as the divisor was not zero, the same will be true. If the divisor 
was zero then an error will be printed and the system will exit. If the two values were not both ints, then an error will be printed and 
the system will exit.

	In the switch statement where the team chose to evaluate expressions, eval_expr, we chose to first complete DynamicDispatch, New, and 
every Internal. In terms of the “hello world” test for P5c, this seemed like a wise decision, but it did slow our progress down when it 
came to getting meaningful output on errors in SLUGS. Let statements, identifiers, blocks, static dispatch, and self dispatch were 
important in providing those errors, so if we were to redo the project perhaps we’d prioritize those over handling String.copy and 
similar expressions. Overall, a significant portion of time was also spent ensuring that the stores were updating correctly and that 
updated stores were used as they came, rather than turning back to old ones (i.e. using sto instead of sto3 where applicable).
	Processing case statements in the interpreter ended up being the most difficult portion after dynamic dispatch. The issues in dynamic 
dispatch were simply due to our initial ignorance in accessing values in the environment and store, but case was difficult because we 
started with a different approach that was a bit harder to debug, comparing inheritance distances rather than opting for recursive upward 
traversal up the lineage, with the base case being either a match for the switch type or reaching the object type, which has no parent. 
We rewrote case handling with the latter format and found improved success. However, we were unable to prevent the stack overflow we  
originally rewrote case for, with suspicions that the problem may lie elsewhere unbeknownst to us at this point.

    In order to correctly label a stack overflow, we needed to keep track of the activation records and throw an error if they surpassed 
1000 at one time. We created a mutable variable using ref that starts at 0 and an update function that takes a bool (true to increase, 
false to decrease) and a line number, such that we can call update to increase the activations at the start of each qualifying evaluation 
(new as well as all forms of dispatch) and the update method checks that an increase does not put us over the activation record limit at 
any point.
    Assignment begins by extracting the name of the identifier and evaluating the expression to which it is assigned. We look up the 
location associated with the identifier in the environment and add the value to that location in the store, and that’s that!

New
    New is one of the cases that counts as an open activation record while it evaluates, so of course we start by incrementing the 
activation counter we discussed earlier. We then get the type we are working with, ensuring that if it is of self type we use the 
type of our self object. We loop up the attributes in the class map and create new locations to store them. We then put the names and 
types of the attributes into two separate lists by folding left on the attributes with switch-based helper functions. These need to be 
together in (name, location) tuples, so we unite them with a double fold left. We then create a new object of the identified type with the 
tupled attributes and new store locations. We need to fill these with either their initialized values or otherwise their default values, 
so we start by getting the default values for all attributes in a list and filling the store with these. We create a block of assignments 
to be evaluated by constructing a list of Assigns for only initialized attributes and evaluating these as a Block type. Finally, we have 
finished the new and can decrement the activation counter.

Let
L   et bindings are to be evaluated as a list of bindings, so we switch on this list of bindings and evaluate the head of the list until 
none remains. Evaluating a single let binding entails getting the name of the variable, its value, and a new store by switching on the 
binding status (whether it is initialized) and either getting the value by evaluation for initialized bindings or the default value for 
non-initialized bindings. A new location is created in the store and the obtained value is added there. The name and location pair are 
added to the environment, and then the expression can be evaluated with this new context of the new environment and store. 

Identifiers
	The most important attribute of an identifier is its name. By switching on the identifier’s name, we determine whether it is a self-type 
or not. If an identifier is “self”, the value associated with it is the current self object, so. Otherwise, the type must be located in 
the environment by searching the environment for the name of the identifier using StringMap.find(name, env). Based on the location returned 
from this find method, we search the location map for that location in order to determine the value at that location. The StringMap.find 
will throw an exception if the name of the identifier is not in the environment (and therefore not in scope). However, assuming it is in 
scope, then searching the store by using LocationMap.find(loc, sto) will either return the value at that location in the store, or throw 
an exception if there is no value at that location (or the location does not exist). Assuming no exceptions are thrown, the Identifier 
evaluation returns the value of the identifier and the current store.

If
	An if expression will contain a predicate, the evaluation of which returns a boolean used to determine whether to evaluate the second 
expression in the statement, or the third. The first evaluation performed with eval_expr then is on the predicate. This evaluation returns 
v1, which is a boolean, as well as sto2, the updated store that includes this value. By switching on the boolean v1, we determine whether 
to evaluate the first or second expression following the predicate, so whether to perform eval_expr on the then_expr, or on else_expr. 
If for some reason, the predicate does not evaluate to a boolean, then we print an error and exit.

While
	A while expression contains both a predicate that evaluates to a boolean, and a body of further expressions to evaluate until the 
predicate does not evaluate to true. The first thing we do, then, is evaluate the predicate with eval_expr, which will return v1, which 
is the boolean the predicate evaluates to, as well as an updated store containing this value. Then, if v1 was true, the body of the while 
expression is evaluated as well, returning an updated store containing all the side effects of the body evaluation. Next, the original 
while expression is evaluated again, this time with the updated store, in essence creating recursion. Once the predicate evaluates to 
false, the value void and newest store are returned. If for some reason the predicate does not evaluate to a boolean, an error is thrown 
and the interpreter exits.

Block
	Block is a special type of expression. Ultimately, the purpose of it is to cycle through each expression in its expression list, and 
collect the store and values returned from them. This is performed using List.fold_left, a method in Reason that takes a function, an 
accumulator, and a list, and applies the function to each element of the list, returning the result to the accumulator variable. The 
function we apply to each expression in the block’s expression list is called eval_seq, which takes an accumulator and an expression, 
isolates the store from the accumulator, and evaluates the expression based on that store in eval_expr. This returns the value resulting 
from that evaluation, as well as the new store containing that result.

Dispatches
	There are three different types of method dispatches in COOL: dynamic, static, and self.  For each of the dispatches the list of 
arguments are evaluated first.  A dispatch_process function was created to produce a list of values associated with each of the 
arguments and the resulting store from evaluating them. Next, the object, v0, that the method is being called on is determined by 
evaluating the e0.  With this object, the type is obtained to get the appropriate function information.  For trivial objects like Int 
and Bool, an empty list is used in place of the attribute list since there are none for these types.  Then, the method information is 
found by using the name of the method with the list of methods for the type of v0 from the implementation map.  Before the body of the 
method can be evaluated, all the attributes and arguments need to be added to the store and environment.  To do so, a get_l function was 
created to make a list of new locations to store the values determined from the arguments each.  These values and locations are loaded 
into the store using the fill_sto_entry on each pair, which returns a new store with a location-value mapping added to the original. 
From the method information, the names of the formals, or arguments, and the newly created locations are added to the environment in a 
similar way with the fill_env_entry function.  
    Since the attributes of v0 are already in the store when it is evaluated, only the environment needs to be updated with the names of the 
attributes and their respective locations.  With all of this completed, the body can be evaluated, and the result of this evaluation is 
returned. Static and self dispatches are very similar to the dynamic dispatch.  The only difference between them is the type used for 
locating the function information.  For static, the specified type is used in place of the type of v0. Self dispatches use the self object 
in place of v0, and as a result, one less store is created throughout the evaluation because there is no e0 to evaluate.  Due to the order 
of the generation of new stores and differing numbers of contents in each dispatches respective subpart, we struggled to find a way to 
neatly package the three up into one function to do eval a dispatch.

Internal
    The internal functions were interesting because we got to implement the functions we have been taking for granted in our use of COOL. 
The method in_int() was the most problematic because of the special cases which must be handled carefully: firstly, that the integer 
may be preceded by whitespace and followed by any characters and integers, but these must be ignored. We originally wrote a terrible 
for loop in an attempt to parse out the integer we wanted, but discovered the Scanf class which does a lot of the work for us, searching 
for the first integer. In order to throw away the rest of the garbage after that, we first read the entire line, and only then applied the 
scanner on that input. Both steps are encased in switch statements in order to catch errors, which evaluate to the integer 0. The other 
special condition is that integers are not too large; if they are, we return an integer of 0 rather than the large value. 
    The in_string() counterpart is fairly different because it has different special cases to handle. We get the entire line of input, but 
we don’t need to throw away much because all integers and characters can be part of that string. However, the entire string must be 
wiped and returned as the empty string if a null character is present in any part. We create a mutable marker and check every character, 
flipping the marker to true if a null character is discovered. This way, we can return the string if the marker is still false after 
iteration, or the empty string if it has been flagged true. If an error occurs in any string processing, as required, the string is again 
set to empty.
    To output integers with the built-in out_int(), we need to look up the function to get its formals. There should only be one, since 
out_int() takes just one parameter (the integer to output), so we look up its value using the environment and store and print that value. 
The built-in out_string is very similar, except that we can’t simply print out the value we obtain. We need to first search for any escaped 
special characters (newline and tab) and replace the escaped versions with the regular versions ReasonML can act upon.
    The abort method is pretty cute, but it wasn’t fully straightforward. We discovered that ReasonML provides a special character to 
induce output flushing, but print_endline also does that all by itself, while even implementing the newline. The type checker doesn’t like 
an exit statement, so we wrapped that in an ignore and pretended to return still.
    COOL creates a shallow copy, such that basic types like integers and strings are copied simply, but more complex object copies do not 
work recursively. We first get the type of the self object. For the dynamic type, this includes the attribute tuple. This tuple could be 
split into attributes names and their locations. We fold left on the attribute locations to build a list of the values we wish to copy by 
looking at the contents of the locations in the current store. We then create a new store location for each attribute and fill them with 
the attribute values we previously extracted. We return a dynamic type of the original object’s name, recombined attribute tuples (using 
the new locations), and the new store.
    The Object.type_name, String.concat, String.length, and String.substr methods are not particularly unique and work as expected, so 
we omit specific explanations here for some semblance of brevity. 

Arithmetic and Comparisons
    Given all of the mathematical operations in COOL follow the same steps to evaluate, a compute_arithmetic function was created. 
This takes in the two expressions from the Add/Sub/Times/Divide exp_subpart variant type, the operation as a function, and divide flag 
to catch division operations. The two expressions are evaluated with appropriate store-threading. The divide flag was added to warn to 
check for potential divide-by-zero errors, as it is the only operation that can produce one.  If there is no error, then the function 
returns a new Int value with the result of the computation along with the store for the last expression evaluation.  In the eval_expr 
function, the bodies for the associated mathematical evaluations were reduced to a single line as a result of functional programming.
The steps followed to evaluate comparisons in COOL are similar to that of the expression for arithmetic.  There are three valid comparisons 
on objects in COOL: less-than, equal-to, and less-than-or-equal to. Each begins with evaluating the two expressions that are to be compared.  For less-than, Ints and Bools are straightforward.  Strings just compare the contents of the string alphabetically using String.compare.  For all other objects and Void, less-than is always false.  Equality is similar to less-than for Strings, Ints, and Bools just with a different operator. For Void objects, they are only equal to another Void and nothing else.  For the Dynamic type, equality only occurs if they are the same object.  To determine this, we added a third attribute to the Dynamic type that is a location.  The objects are equal if their locations are.  Less-than-or-equal-to is just a combination of the previous two, as suggested by its name.
Other similar expressions that were evaluated include Isvoid, Not and Negate.  Isvoid is not very complex; a boolean value was returned 
depending on whether the specified object matched the “Void” value type. Not and Negate are similar operations for Bool and Int types, 
respectively.  Not takes the boolean from the specified object, and returns the opposite boolean, and Negate takes the integer and returns 
a new integer multiplied by -1.
	
Case
    To begin the case evaluation, the case expression is evaluated and the type of the result, x_type, is determined with the getType 
function.  Two helper functions were made to process the case branches: same_as_x and get_branch.  The first takes a type and a list of 
case branches and returns a list of the branches that have the same type as the one specified.  This is then used in get_branch.  This 
function takes the same parameters.  It calls same_as_x and if the list is empty, then there were no matching branches.  When there is no 
match, there is a recursive call with the same list of elements but with the parent of x_type, unless it is “Object” and it has no parent. 
In this case, there were no branches that satisfied the case expression and then the “case without matching branch” error is produced. 
If same_as_x has an element in it, then a match was found, and this is the branch to evaluate.  The value of the case expression is put 
into the store with a new location, and the identifier from the matched branch is stored in the environment with the same location.  
Lastly, the branch expression is evaluated and the result is returned at the result of evaluating the entire case expression.

Test Cases Decisions-----------------------------------------------------------------------------------------
	We started our test case suite with the example COOL programs provided by the course. We then began our targeted approach, formulating 
tests that specifically addressed certain runtime exceptions and error handling identified by the COOL Reference Manual.

Test 1: primarily tests math expressions, dispatch on void, divide by zero, scoping issues with the variable y, outstring, static dispatch, 
        if statements, recursion, and inheritance
Test 2: primarily tests inheritance, self dispatch, uninstantiated variables, initialization, internal methods, while loops, “not”, 
        case statements, comparisons of non-basic types
Test 3: primarily tests empty class that inherits, main not in a Main class, scoping, in_int() of blank line followed by number, and 
        out_int() of identifiers
Test 4: primarily tests block assignment statement, scoping and shallow copying in assignments, out_int on identifiers, self dispatch, 
        “new”.init statements, out_ints on identifiers performing dispatches
Test 5: primarily tests out_string of a dispatch expression, string and int methods, if statements, abort, inheritance
Test 6: primarily tests handling of null character in in_string and out_string
Test 7: primarily tests handling of a blank file and blank input file
Test 8: primarily tests the handling of a relatively complex, real COOL program (Tom and Kim’s earlier project from this course) with 
        significant string input; creation of Lists, while loops, list length, in_string, identifier comparison to booleans and strings, 
        variety of return types, scoping and incrementing counters, initialize with arguments, self type
Test 10: BIG case, use of many, many types, Object, String, Int, assignments, comparisons, internal expressions, string methods such as 
        substr, cases, lots of scoping
Test 16: primarily tests main returns a SELF_TYPE not Object, out_string of strings and identifiers, if-then-else statements, “not”
Test 17: primarily tests main returns an Int but the main code does not occur in main, out_string, while loop, comparisons with math
Test 23: Adapted from the CRM, primarily tests class creation, inheritance to a blank class, (shallow) copy method, out_int of identifiers 
        with a dispatch
Test 28: primarily tests division by 0, newlines and tabs, initializing to default values, isvoid, out_string of white space
Test 97: primarily tests init() and init with attributes, type_name, comparisons of a number of different types of objects, out_int of 
        dispatch expressions, comparing to objects of empty class
Test 98: primarily tests the maximum integer size, as well as in_int when parsing characters before integers, stack overflow, adding 
        two identifiers, comparing identifier to int, incrementing counters
Test 99: primarily tests white space before string in input for in_string, stack overflow, repeated concatenation




