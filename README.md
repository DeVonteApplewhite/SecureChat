SecureChat
==========

Implements various cryptographical protocols used in secure chat clients
________________________________________________________________________

Contains the following four cryptographic primitives:
* Perfect forward secrecy
  * Ensure past messages cannot be recovered retroactively
* Digital signatures
  * A user knows with whom he/she is communicating
* Message authentication codes
  * Prove that the other party's authorship of a message to the user,
    while at the same time preventing such a proof to third parties.
* Malleable encryption
  * Provide for forgeability of transcripts, repudiation of contents,
    and plausible deniability.

To compile the project, run the following command:
make -f makefile
This should produce a main executable.
