## Two Phase Commit �Ɣ�r���� Paxos �̒��� ##
Paxos��2�̏d�v�ȃ��J�j�Y���� 2PC �ɉ����Ă���B

1. ��Ăɏ����t�����s�����ƂŗႦ��2�̒�Ă̂����ǂ�������F����ׂ���������ł��邱�ƁB
2. Acceptor�̉ߔ��������F�����肵�����Ƃ��������Ƃ��ɒ�Ă��󂯓����ꂽ�ƍl���邱�ƁB(2PC�͑S�Ă�acceptor�����ӂ�����������Ă����F�����)



## Basic Paxos ##
���̃v���g�R���̓p�N�\�X�t�@�~���[�̍ł���{�I�Ȃ��̂ł���B
�x�[�V�b�N�p�N�\�X�v���g�R���̊e�C���X�^���X�͒P��̏o�͒l�����肷��B
���̃v���g�R���͊���̃��E���h�Ői�s����B
����n�̃��E���h��2�̃t�F�[�Y����Ȃ�B
�v���|�[�U�[�́A�A�N�Z�v�^�[�B��(���Ȃ��Ƃ�)�葫���ƒʐM���o���Ȃ��Ȃ�A�p�N�\�X���J�n���Ȃ��B

### �t�F�[�Y 1a: *����* ###

�v���|�[�U�[(���[�_�[)�͔ԍ� N �Ŏ��ʂ�����Ă𐶐�����B
N �� ���̃v���|�[�U�[���ߋ��Ɏg�p������Ĕԍ����傫���Ȃ���΂Ȃ�Ȃ��B
�v���|�[�U�[�́A���̒�Ă��܂� *����* ���b�Z�[�W���A�N�Z�v�^�[�̒葫���֑��M����B

<!--
=== Phase 1a: ''Prepare'' ===

: A [[#Proposer|Proposer]] (the [[#Leader|leader]]) creates a proposal identified with a number N. This number must be greater than any previous proposal number used by this Proposer. Then, it sends a ''Prepare'' message containing this proposal to a [[#Quorums|Quorum]] of [[#Acceptor|Acceptors]].
-->

### �t�F�[�Y 1b: *��* ###

�A�N�Z�v�^�[�́A�C�ӂ̃v���|�[�U�[�����M�����ߋ��̔C�ӂ̒�Ĕԍ����傫���ԍ� N �̒�Ă���M������A�u�����M���� N��菬�����ԍ��̒�Ă𖳎�����v�Ƃ��� *��* ��Ԃ��B�A�N�Z�v�^�[���ߋ��ɂ����Ă��󂯓���Ă����Ȃ�A�v���|�[�U�[�ւ̕ԓ��͑O�̒�Ĕԍ��ƑO�̒l���܂܂Ȃ���΂Ȃ�Ȃ��B

�����Ȃ���(�܂�A�ߋ��Ɏ�M������Ĕԍ���菬���Ȕԍ� N �̒�Ă���M������)�A�A�N�Z�v�^�[�͎�M������Ă𖳎����Ă悢�B���̏ꍇ �p�N�\�X���@�\���邽�߂ɂ́A�A�N�Z�v�^�[�͓�����K�v�͂Ȃ��B�������A���ۉ���(NAK)�𑗐M���āA�v���|�[�U�[�ɒ��N�̍��ӌ`�����~�߂Ă悢�Ɠ`���邱�Ƃ� �œK����}�邱�Ƃ��ł���B(�œK�� = �v���|�[�U�[������ȏ� ���N�ɌŎ������ɍς�)

<!--
=== Phase 1b: ''Promise'' ===

: If the proposal's number N is higher than any previous proposal number received from any Proposer by the Acceptor, then the Acceptor must return a promise to ignore all future proposals having a number less than N. If the Acceptor accepted a proposal at some point in the past, it must include the previous proposal number and previous value in its response to the Proposer.

: Otherwise, the Acceptor can ignore the received proposal. It does not have to answer in this case for Paxos to work. However, for the sake of optimization, sending a denial (''[[NAK (protocol message)|Nack]]'') response would tell the Proposer that it can stop its attempt to create consensus with proposal N.
-->

### �t�F�[�Y 2a: *�v���̎󂯓���*

�v���|�[�U�[���A�N�Z�v�^�[�̒葫������\���Ȑ��� *��* ����M������A����l�����̒�Ăɐݒ肷��K�v������B�C�ӂ̃A�N�Z�v�^�[�B�� �������̒�Ă����Ɏ󂯓���Ă�����A�����A�N�Z�v�^�[�B�̓v���|�[�U�[�ւ�����(�󂯓��ꂽ��Ă�)�l�𑗂�A�v���|�[�U�[�͂��ꂩ�� ���g����Ă���l���A�N�Z�v�^�[�B�ɂ���ĕ񍐂��ꂽ�ō��̒�Ĕԍ��ɕR�Â��l�ɐݒ肵�Ȃ���΂Ȃ�Ȃ��B���̎��_�܂ł� ��Ă��󂯓��ꂽ�A�N�Z�v�^�[�����Ȃ������Ȃ�A�v���|�[�U�[�͂��̒�ĂɔC�ӂ̒l��I��ł悢�B

�v���|�[�U�[�� ���̒�ĂƂ��đI�����ꂽ�l�Ƌ��� *�󂯓���v��* ���b�Z�[�W�� �A�N�Z�v�^�[�̒葫���� ���M����B


<!--
=== Phase 2a: ''Accept Request'' ===
: If a Proposer receives enough promises from a Quorum of Acceptors, it needs to set a value to its proposal. If any Acceptors had previously accepted any proposal, then they'll have sent their values to the Proposer, who now must set the value of its proposal to the value associated with the highest proposal number reported by the Acceptors. If none of the Acceptors had accepted a proposal up to this point, then the Proposer may choose any value for its proposal.<ref name="paxos-simple"/>

: The Proposer sends an ''Accept Request'' message to a Quorum of Acceptors with the chosen value for its proposal.
-->

### �t�F�[�Y 2b: *�󂯓��ꊮ��*
�A�N�Z�v�^�[�� ��� N �� �󂯓���v�����b�Z�[�W ����M������A
�u�A�N�Z�v�^�[�� N ���傫�����ʎq������Ă݂̂��󂯓����Ɗ��ɖ񑩂��Ă����Ȃ�A�����̎��Ɍ���v���̒�Ă��󂯓��Ȃ���΂Ȃ�Ȃ��B
���̏ꍇ�A�֌W����l v ��o�^���A*�󂯓��ꊮ��* ���b�Z�[�W���v���|�[�U�[�Ɗe���[�i�[(�w�K��)�֑��M����B
�����łȂ��ꍇ�A�A�N�Z�v�^�[�͂��̎󂯓���v���𖳎����Ă悢�B

�����̃v���|�[�U�[���������� *����* ���b�Z�[�W�𑗐M����Ƃ��A���邢�́A�v���|�[�U�[���ԐM(*��* �������� *�󂯓��ꊮ��*)�̒葫������M���Ȃ��Ƃ���(������)���E���h�͎��s����B�����̏ꍇ�A��荂����Ĕԍ��̃��E���h���J�n����Ȃ���΂Ȃ�Ȃ��B

�A�N�Z�v�^�[���v�����󂯓����Ƃ��A�A�N�Z�v�^�[�̓v���|�[�U�[�̃��[�_�[�V�b�v��F�����Ă��邱�Ƃɒ��ӁB�]���āA�p�N�\�X�͕����̃m�[�h����Ȃ�N���X�^���̃��[�_�[��I�����邽�߂ɗp���邱�Ƃ��ł���B

���}�� Basic Paxos �v���g�R����\���Ă���B�ŏ��̒�Ă��Ȃ����Ƃ��A���̃��E���h�ňȑO�ɒl���󂯓���Ă��A�N�Z�v�^�[�����Ȃ����߁A *��* ���b�Z�[�W�Ɋ܂܂�ĕԂ����l�� null �ł��邱�Ƃɒ��ӁB 

<!--
=== Phase 2b: ''Accepted'' ===

: If an Acceptor receives an Accept Request message for a proposal N, it must accept it [[if and only if]] it has not already promised to only consider proposals having an identifier greater than N. In this case, it should register the corresponding value v and send an ''Accepted'' message to the Proposer and every Learner. Else, it can ignore the Accept Request.

: Rounds fail when multiple Proposers send conflicting ''Prepare'' messages, or when the Proposer does not receive a Quorum of responses (''Promise'' or ''Accepted'').  In these cases, another round must be started with a higher proposal number.

: Notice that when Acceptors accept a request, they also acknowledge the leadership of the Proposer. Hence, Paxos can be used to select a leader in a cluster of nodes.

: Here is a graphic representation of the Basic Paxos protocol.  Note that the values returned in the ''Promise'' message are null the first time a proposal is made, since no Acceptor has accepted a value before in this round.
-->

=== Message flow: Basic Paxos ===

<small>(first round is successful)</small>
<pre>
 Client   Proposer      Acceptor     Learner
   |         |          |  |  |       |  |
   X-------->|          |  |  |       |  |  Request
   |         X--------->|->|->|       |  |  Prepare(1)
   |         |<---------X--X--X       |  |  Promise(1,{Va,Vb,Vc})
   |         X--------->|->|->|       |  |  Accept!(1,Vn)
   |         |<---------X--X--X------>|->|  Accepted(1,Vn)
   |<---------------------------------X--X  Response
   |         |          |  |  |       |  |
</pre>
Vn = last(Va,Vb,Vc)

### Basic Paxos �ɂ�����G���[�P�[�X

�ł��P���ȃG���[�P�[�X�́A�璷���[�i�[(�w�K��)�̌̏�A���邢�� �A�N�Z�v�^�[�̒葫�����������Ă���󋵂ł̃A�N�Z�v�^�[�̌̏�ł���B
�����̃P�[�X�ŁA���̃v���g�R����(�̏Ⴉ���)�񕜂��s�v�ł���B
���L�Ɍ�����ʂ�A�ǉ��I�ȃ��E���h��A���b�Z�[�W���s�v�ł���B

<!--
=== Error cases in basic Paxos ===

The simplest error cases are the failure of a redundant Learner, or failure of an Acceptor when a Quorum of Acceptors remains live.  In these cases, the protocol requires no recovery.  No additional rounds or messages are required, as shown below:
-->


=== Message flow: Basic Paxos, failure of Acceptor ===

<small>(Quorum size = 2 Acceptors)</small>
<pre>
 Client   Proposer      Acceptor     Learner
   |         |          |  |  |       |  |
   X-------->|          |  |  |       |  |  Request
   |         X--------->|->|->|       |  |  Prepare(1)
   |         |          |  |  !       |  |  !! FAIL !!
   |         |<---------X--X          |  |  Promise(1,{null,null, null})
   |         X--------->|->|          |  |  Accept!(1,V)
   |         |<---------X--X--------->|->|  Accepted(1,V)
   |<---------------------------------X--X  Response
   |         |          |  |          |  |
</pre>

=== Message flow: Basic Paxos, failure of redundant Learner ===

<pre>
 Client   Proposer      Acceptor     Learner
   |         |          |  |  |       |  |
   X-------->|          |  |  |       |  |  Request
   |         X--------->|->|->|       |  |  Prepare(1)
   |         |<---------X--X--X       |  |  Promise(1,{null,null,null})
   |         X--------->|->|->|       |  |  Accept!(1,V)
   |         |<---------X--X--X------>|->|  Accepted(1,V)
   |         |          |  |  |       |  !  !! FAIL !!
   |<---------------------------------X     Response
   |         |          |  |  |       |
</pre>

���̌̏�P�[�X��


<!--
The next failure case is when a Proposer fails after proposing a value, but before agreement is reached.  Ignoring Leader election, an example message flow is as follows:
-->
=== Message flow: Basic Paxos, failure of Proposer ===

<small>(re-election not shown, one instance, two rounds)</small>
<pre>
Client  Proposer        Acceptor     Learner
   |      |             |  |  |       |  |
   X----->|             |  |  |       |  |  Request
   |      X------------>|->|->|       |  |  Prepare(1)
   |      |<------------X--X--X       |  |  Promise(1,{null, null, null})
   |      |             |  |  |       |  |
   |      |             |  |  |       |  |  !! Leader fails during broadcast !!
   |      X------------>|  |  |       |  |  Accept!(1,Va)
   |      !             |  |  |       |  |
   |         |          |  |  |       |  |  !! NEW LEADER !!
   |         X--------->|->|->|       |  |  Prepare(2)
   |         |<---------X--X--X       |  |  Promise(2,{null, null, null})
   |         X--------->|->|->|       |  |  Accept!(2,V)
   |         |<---------X--X--X------>|->|  Accepted(2,V)
   |<---------------------------------X--X  Response
   |         |          |  |  |       |  |
</pre>

The most complex case is when multiple Proposers believe themselves to be Leaders.  For instance the current leader may fail and later recover, but the other Proposers have already re-elected a new leader.  The recovered leader has not learned this yet and attempts to begin a round in conflict with the current leader.

=== Message flow: Basic Paxos, dueling Proposers ===

<small>(one instance, four unsuccessful rounds)</small>
<pre>
Client   Leader         Acceptor     Learner
   |      |             |  |  |       |  |
   X----->|             |  |  |       |  |  Request
   |      X------------>|->|->|       |  |  Prepare(1)
   |      |<------------X--X--X       |  |  Promise(1,{null,null,null})
   |      !             |  |  |       |  |  !! LEADER FAILS
   |         |          |  |  |       |  |  !! NEW LEADER (knows last number was 1)
   |         X--------->|->|->|       |  |  Prepare(2)
   |         |<---------X--X--X       |  |  Promise(2,{null,null,null})
   |      |  |          |  |  |       |  |  !! OLD LEADER recovers
   |      |  |          |  |  |       |  |  !! OLD LEADER tries 2, denied
   |      X------------>|->|->|       |  |  Prepare(2)
   |      |<------------X--X--X       |  |  Nack(2)
   |      |  |          |  |  |       |  |  !! OLD LEADER tries 3
   |      X------------>|->|->|       |  |  Prepare(3)
   |      |<------------X--X--X       |  |  Promise(3,{null,null,null})
   |      |  |          |  |  |       |  |  !! NEW LEADER proposes, denied
   |      |  X--------->|->|->|       |  |  Accept!(2,Va)
   |      |  |<---------X--X--X       |  |  Nack(3)
   |      |  |          |  |  |       |  |  !! NEW LEADER tries 4
   |      |  X--------->|->|->|       |  |  Prepare(4)
   |      |  |<---------X--X--X       |  |  Promise(4,{null,null,null})
   |      |  |          |  |  |       |  |  !! OLD LEADER proposes, denied
   |      X------------>|->|->|       |  |  Accept!(3,Vb)
   |      |<------------X--X--X       |  |  Nack(4)
   |      |  |          |  |  |       |  |  ... and so on ...
</pre>
