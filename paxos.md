## Two Phase Commit と比較した Paxos の長所 ##
Paxosは2つの重要なメカニズムを 2PC に加えている。

1. 提案に順序付けを行うことで例えば2つの提案のうちどちらを承認するべきかが決定できること。
2. Acceptorの過半数が承認を決定したことを示したときに提案が受け入れられたと考えること。(2PCは全てのacceptorが同意した時だけ提案が承認される)



## Basic Paxos ##
このプロトコルはパクソスファミリーの最も基本的なものである。
ベーシックパクソスプロトコルの各インスタンスは単一の出力値を決定する。
このプロトコルは幾つかのラウンドで進行する。
正常系のラウンドは2つのフェーズからなる。
プロポーザーは、アクセプター達の(少なくとも)定足数と通信が出来ないなら、パクソスを開始しない。

### フェーズ 1a: *準備* ###

プロポーザー(リーダー)は番号 N で識別される提案を生成する。
N は このプロポーザーが過去に使用した提案番号より大きくなければならない。
プロポーザーは、この提案を含む *準備* メッセージをアクセプターの定足数へ送信する。

<!--
=== Phase 1a: ''Prepare'' ===

: A [[#Proposer|Proposer]] (the [[#Leader|leader]]) creates a proposal identified with a number N. This number must be greater than any previous proposal number used by this Proposer. Then, it sends a ''Prepare'' message containing this proposal to a [[#Quorums|Quorum]] of [[#Acceptor|Acceptors]].
-->

### フェーズ 1b: *約束* ###

アクセプターは、任意のプロポーザーから受信した過去の任意の提案番号より大きい番号 N の提案を受信したら、「今後受信する Nより小さい番号の提案を無視する」という *約束* を返す。アクセプターが過去にある提案を受け入れていたなら、プロポーザーへの返答は前の提案番号と前の値を含まなければならない。

さもなくば(つまり、過去に受信した提案番号より小さな番号 N の提案を受信したら)、アクセプターは受信した提案を無視してよい。この場合 パクソスが機能するためには、アクセプターは答える必要はない。しかし、拒否応答(NAK)を送信して、プロポーザーに提案Nの合意形成を止めてよいと伝えることで 最適化を図ることができる。(最適化 = プロポーザーがこれ以上 提案Nに固執せずに済む)

<!--
=== Phase 1b: ''Promise'' ===

: If the proposal's number N is higher than any previous proposal number received from any Proposer by the Acceptor, then the Acceptor must return a promise to ignore all future proposals having a number less than N. If the Acceptor accepted a proposal at some point in the past, it must include the previous proposal number and previous value in its response to the Proposer.

: Otherwise, the Acceptor can ignore the received proposal. It does not have to answer in this case for Paxos to work. However, for the sake of optimization, sending a denial (''[[NAK (protocol message)|Nack]]'') response would tell the Proposer that it can stop its attempt to create consensus with proposal N.
-->

### フェーズ 2a: *要求の受け入れ*

プロポーザーがアクセプターの定足数から十分な数の *約束* を受信したら、ある値をその提案に設定する必要がある。任意のアクセプター達が 何等かの提案を既に受け入れていたら、それらアクセプター達はプロポーザーへそれらの(受け入れた提案の)値を送り、プロポーザーはこれから 自身が提案する値をアクセプター達によって報告された最高の提案番号に紐づく値に設定しなければならない。この時点までに 提案を受け入れたアクセプターがいなかったなら、プロポーザーはその提案に任意の値を選んでよい。

プロポーザーは その提案として選択された値と共に *受け入れ要求* メッセージを アクセプターの定足数へ 送信する。


<!--
=== Phase 2a: ''Accept Request'' ===
: If a Proposer receives enough promises from a Quorum of Acceptors, it needs to set a value to its proposal. If any Acceptors had previously accepted any proposal, then they'll have sent their values to the Proposer, who now must set the value of its proposal to the value associated with the highest proposal number reported by the Acceptors. If none of the Acceptors had accepted a proposal up to this point, then the Proposer may choose any value for its proposal.<ref name="paxos-simple"/>

: The Proposer sends an ''Accept Request'' message to a Quorum of Acceptors with the chosen value for its proposal.
-->

### フェーズ 2b: *受け入れ完了*
アクセプターは 提案 N の 受け入れ要求メッセージ を受信したら、
「アクセプターが N より大きい識別子を持つ提案のみを受け入れると既に約束していたなら、かつその時に限り」その提案を受け入なければならない。
この場合、関係する値 v を登録し、*受け入れ完了* メッセージをプロポーザーと各リーナー(学習者)へ送信する。
そうでない場合、アクセプターはその受け入れ要求を無視してよい。

複数のプロポーザーが相反する *準備* メッセージを送信するとき、あるいは、プロポーザーが返信(*約束* もしくは *受け入れ完了*)の定足数を受信しないときに(複数の)ラウンドは失敗する。これらの場合、より高い提案番号のラウンドが開始されなければならない。

アクセプターが要求を受け入れるとき、アクセプターはプロポーザーのリーダーシップを認識していることに注意。従って、パクソスは複数のノードからなるクラスタ内のリーダーを選択するために用いることができる。

下図は Basic Paxos プロトコルを表している。最初の提案がなされるとき、このラウンドで以前に値を受け入れてたアクセプターがいないため、 *約束* メッセージに含まれて返される値は null であることに注意。 

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

### Basic Paxos におけるエラーケース

最も単純なエラーケースは、冗長リーナー(学習者)の故障、あるいは アクセプターの定足数が生存している状況でのアクセプターの故障である。
これらのケースで、このプロトコルは(故障からの)回復が不要である。
下記に見られる通り、追加的なラウンドや、メッセージも不要である。

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

次の故障ケースは


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
