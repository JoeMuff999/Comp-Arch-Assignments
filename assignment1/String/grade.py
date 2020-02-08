from subprocess import check_output


tests = [
    ("strlen", "helloworld"),
    ("memmove",),
    ("strcpy", "Hello",),
    ("strncpy", "Hello", "5",),
]

#
# As you do the assignment, you can put more tests into the tests array.
# Here are a few. Don't forget to separate them with a comma
#
#    ("strncpy", "Hello_World", 5,)
#    ("memmove",),  #memmove test takes no parameter
#    ("memmove",),  #the comma inside the tuple is important


for test in tests:
    cmd = ["./grade"] + list(test)
    print("test: {}    {}".format(test, "CORRECT" if "1" in str(check_output(cmd)) else "WRONG"))

