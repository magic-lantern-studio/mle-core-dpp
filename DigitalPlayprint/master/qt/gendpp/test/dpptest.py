print("Digital Playprint Test Application")

import dpp
status = dpp.system("ls -l")

dpp.codefile("test")
dpp.beginplayprint("test.dpp")
dpp.endplayprint()
