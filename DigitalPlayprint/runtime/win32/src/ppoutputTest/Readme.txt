This directory contains a unittest for the MleDppOutput class. The input
for the test is "test.dwp", a Digital Workprint. test.dwp references the
following Digital Workprint files: Mle3dForum.wpf, Mle3dCameraActor.wpd,
MleEscapeActor.wpd, MySpinnerActor.wpd.


Running the test generates the following files: actorid.h, groupid.h, g0.chk,
and testle.dpp, a Digital Playprint in Little Endian format containing the
Actor Group Chunk contained in the g0.chk file.
