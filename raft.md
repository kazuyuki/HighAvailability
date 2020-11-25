# 分かりやすい合意アルゴリズムを求めて

##概要

1. 紹介

ラフトはログの複製を管理するための合意アルゴリズムです。

	* 強いリーダー
	* リーダー選挙
	* メンバー変更

2. ステートマシンの複製

3. パクソスの何が悪いのか

4. 理解性のためのデザイン

5. ラフト合意アルゴリズム

ラフトは2章で示した形の複製ログを管理するためのアルゴリズムです。図2は、参考用にアルゴリズムを凝縮してまとめたもので、図3はアルゴリズムの重要な特性の一覧です。これらの図の要素については、このセクションの残りの部分では個別に説明します。

ラフトは最初に特別な *リーダー* を選出し、複製ログの管理に対する完全な責任を与えることで合意を行います。

リーダーはクライアントからのログエントリを受け付け、それを他のサーバー群に複製し、サーバ群の各々が持つステートマシンにログエントリを適用することが安全になったら サーバ群へ通知を行います。リーダーを持つことは複製ログの管理を単純化します。例えば、リーダーは他のサーバ群と相談せずに ログのどこに新しいエントリを置くかを決定でき、データはリーダーから他のサーバ群へ単純な方法で受け渡されます。 リーダーは障害が起こったり、他のサーバ群から切断されることがありますが、そのときは新たなリーダーが選ばれます。

リーダーアプローチによってラフトは合意問題を後の節で説明する3つの比較的独立した部分問題に分解します。

	* リーダー選挙 : 新たなリーダは既存のリーダが障害となったときに選ばれる必要があります。(5.2章)
	* ログ複製 : リーダーはクライアントからログエントリーを受け付け、それをクラスタ全体に複製し、自身と他のログとを強制的に一致させる必要があります。(5.3章)
	* 安全性 :　Raftにとって鍵となる安全性属性は図3　にある State Machine Safety Property である。
	もし任意のサーバが自身のステートマシンに特定のログエントリを摘要するなら、
	同じログインデックスに対して異なるコマンドを適用してよい他のサーバは無い。

--------
**図2** : ラフト コンセンサスアルゴリズムの要約 (メンバーシップ変更とログ圧縮を除く)  

「状態」に書かれている動作は、独立かつ反復して適用されるルールの集合として記述されている。
5.2節のような節番号は特定の機能が議論されている場所を示している。
より正確なアルゴリズムは形式仕様 [31] に説明がある。

# 状態

## 全サーバーの永続的状態 :
(RPCに応答する前に安定したストレージ上で更新される)

	currentTerm	サーバーが見る 最新の期間 (初回起動時に 0 で初期化され、単調増加する)
	votedFor	現在の期間で投票を受信した候補者ID (誰もいなければ null)
	log[]		ログエントリー。各エントリーはステートマシンに対するコマンドを含んでおり、リーダーによってエントリーが受信されたら終了する (最初のインデックスは 1)

## 全サーバの揮発的状態 :

	commitIndex	コミットされたログエントリーのインデックスの最大値 (0で初期化され、単調増加する)
	lastApplied	ステートマシンに適用されたログエントリーの最大のインデックス (0で初期化され、単調増加する)

## リーダーの揮発的状態 :
(選挙後に再初期化される)

	nextIndex[]	各サーバーに送信される次のログエントリーのインデックス (リーダーの最新のログインデックス +1 に初期化される)
	matchIndex[]	各サーバーが複製されたと認識するログエントリーの最大のインデックス (0で初期化され、単調増加する)


--------
# AppendEntries RPC (エントリー追加)
ログエントリーを複製するためにリーダーが実行する(5.3節) またハートビートにも使用される。

## 引数

	term    	リーダーの任期番号
	leaderId	フォロワーがクライアントをリダイレクトするために使う
	prevLogIndex	新しいログエントリーに先行するログエントリーのインデックス
	prevLogTerm	prevLogindex エントリーの期間番号
	entries[]	保存するログエントリー(ハートビートの時は空。有効性のために1回以上の送信があってよい)
	leaderCommit	リーダーの commitIndex

## 結果

	term	リーダーが自身を更新するための currentTerm
	success	フォロワーが prevLogIndex と prevLogTerm にマッチするエントリーを保存した場合 true となる

## 受信側の実装

	1. term < currentTerm なら false を返信する (5.1節)
	2. ログが「term が prevLogTerm と等しい、prevLogIndex 番目のエントリ」を持たないとき false を返信する (5.3節)
	3. 既存エントリーが新しいものとコンフリクトする(インデックスが同じだが term が異なる)なら、既存のエントリーとそれに続く全てのエントリーを削除する。
	4. ログに無い 新たなエントリーは いかなるものでも 追加する
	5. leaderCommit > commitIndex なら、commitIndex = min( leaderCommit, 最後の新エントリーのインデックス)

--------
# RequestVote RPC (投票依頼)
投票を集めるために候補者によって呼び出される

## 引数

	term    	候補者の期間番号
	cadidateId	投票を要求する候補者
	lastLogIndex	候補者の最後のログエントリーのインデックス(5.4節)
	lastLogTerm	候補者の最後のログエントリーの期間番号(5.4節)

## 結果

	term		候補者が currentTerm をこれで更新する
	voteGranted	true は候補者が投票を受信したことを意味する。

## 受信側の実装

	1. term < currentTerm なら false を返信する。(5.1節)
	2. votedFor が null 若しくは candidateId、かつ、候補者のログが少なくとも受信者のログと同等に最新なら、投票権を付与する。(5.2, 5.4節)

--------
# サーバーの規則

## 全てのサーバー

	* commitIndex > lastApplied なら lastApplied をインクリメントし、ステートマシンに log[lastApplied] を適用する。
	* RPC要求または応答が currentTerm より大きい 任期番号T を持つ (T > currentTerm) なら currentTerm に T をセットして、フォロワーになる。 (5.1節)

## フォロワー (5.2節)

	* 候補者とリーダーからのRPCに応答する
	* 現在のリーダーからの AppendEntries RPC の受信、あるいは、候補者投票権の付与を受信せずに選挙タイムアウトが経過したら、候補者になる。

## 候補者 (5.2節)

	* 候補者との会話では、選挙を始めるとき
	  * currentTerm をインクリメントする
	  * 自身に投票する
	  * 選挙タイマーをリセットする
	  * 投票依頼 RPC を全ての他のサーバへ送信する
	* サーバーの多数派から投票を受信したらリーダーになる
	* 新たなリーダーから エントリー追加 RPC を受信したらフォロワーになる
	* 選挙タイムアウトが経過したら新たな選挙を始める

## リーダー

	* 選挙後、最初に空の エントリー追加 RPC (ハートビート) を各サーバーへ送信する。選挙タイムアウトを防ぐために、休眠期間毎に継続する。
	* クライアントからコマンドを受信したら、ローカルログにエントリーを追加して、ステートマシンにエントリーを適用後、返信する。(5.3節)
	* 「最後の ログのインデックス」 > 「フォロワーに対する nextIndex」ならエントリー追加 RPC をnextIndex で始まる ログエントリーと一緒に送る。
	  * 成功したら、フォロワーに対する nextIndex と matchindex を更新する(5.3節)
	  * ログに一貫性が無く エントリー追加 が失敗したら、nextIndex をデクリメントしてリトライする(5.3節)
	* N > commitIndex , 「matchIndex[i] の多数派」≧ N , かつ, log[N].term == currentTerm となるような N が存在するなら、 commitIndex に N をセットする (5.3節、5.4節)

--------

**図3** : Raftはこれら属性の各々が常に真であることを保証する。章番号は各属性が説明されている場所を示している。

Election Safety :与えられた任期において、選出可能なリーダは 高々一人である。

Leader Append-Only : リーダは自信のログ内のエントリに対して、決して上書きや削除を行わなず、新たなエントリを追加するのみである。

Log Matching : 同じ任期、同じインデックスのエントリが2つのログに含まれているなら、そのログはインデックス番号1から与えられたインデックスまでの全てのエントリについて同一である。 (5.3節)

Leader Completeness : 与えられた任期でログエントリがコミットされたら、そのエントリは より高い番号の全ての任期のリーダーのログにも存在する。

State Machine Safety : 与えられたインデックスのログエントリをサーバがステートマシンに適用したら、他のサーバは同じインデックスに対して異なるログエントリを決して適用しない。(5.4.3節)

--------

5.1 ラフトの基本

5.2 リーダー選挙
Raftはリーダー選挙を行うために、ハートビートの仕組みを使う。サーバが走り始めたら、それらは まずフォロワーになる。
サーバはリーダーか候補者から有効なRPCを受信するまでフォロワーの状態のままです。
リーダー達は全フォロワーへ定期的なハートビート（ログエントリーの無い AppendEntriesRPC）を送信し彼らの権威を維持する。
フォロワーが *ｅｌｅｃｔｉｏｎ ｔｉｍｅｏｕｔ*　と呼ばれる期間、何も受信しなかった場合、動作可能なリーダーが存在しないと見做して新たなリーダーを選ぶために選挙を始める。

フォロワーは選挙を始める際、ｃｕｒｒｅｎｔ ｔｅｒｍをインクリメントし、候補者状態になる。候補者状態になったら自分自身に投票し、平行してクラスタ内の他のサーバに RequestVore　RPC　を送信する。
候補者は下記 3つの内 1つが起こるまでこの状態維持する。

(a)選挙に勝つ (b)他のサーバがリーダーになる (c)勝者がいないまま ある期間が過ぎる

これらの成果を以下に説明する。

A candidate wins an election if it receives votes from
a majority of the servers in the full cluster for the same
term. Each server will vote for at most one candidate in a
given term, on a first-come-first-served basis (note: Sec-
tion 5.4 adds an additional restriction on votes). The ma-
jority rule ensures that at most one candidate can win the
election for a particular term (the Election Safety Prop-
erty in Figure 3). Once a candidate wins an election, it
becomes leader. It then sends heartbeat messages to all of
the other servers to establish its authority and prevent new
elections.

While waiting for votes, a candidate may receive an
AppendEntries RPC from another server claiming to be
leader. If the leader’s term (included in its RPC) is at least
as large as the candidate’s current term, then the candidate
recognizes the leader as legitimate and returns to follower
state. If the term in the RPC is smaller than the candidate’s
current term, then the candidate rejects the RPC and con-
tinues in candidate state.

3つ目に得られる結果は
もし、多くのフォロワーが同時に候補者になったら、投票が分裂するので、多数派（の投票）を獲得する候補者がいなくなり、候補者が選挙を勝ったり負けたりしなくなるということです。
この状況が起こると、候補者はタイムアウトします。そして ｔｅｒｍをインクリメントし、新たなラウンドの RequestVote　RPC　を開始することによって新たな選挙を始めます。

Raft uses randomized election timeouts to ensure that
split votes are rare and that they are resolved quickly. To
prevent split votes in the first place, election timeouts are
chosen randomly from a fixed interval (e.g., 150–300ms).
This spreads out the servers so that in most cases only a
single server will time out; it wins the election and sends
heartbeats before any other servers time out. The same
mechanism is used to handle split votes. Each candidate
restarts its randomized election timeout at the start of an
election, and it waits for that timeout to elapse before
starting the next election; this reduces the likelihood of
another split vote in the new election. Section 9.3 shows
that this approach elects a leader rapidly.
Figure 6: Logs are composed of entries, which are numbered
sequentially. Each entry contains the term in which it was
created (the number in each box) and a command for the state
machine. An entry is considered committed if it is safe for that
entry to be applied to state machines.
Elections are an example of how understandability
guided our choice between design alternatives. Initially
we planned to use a ranking system: each candidate was
assigned a unique rank, which was used to select between
competing candidates. If a candidate discovered another
candidate with higher rank, it would return to follower
state so that the higher ranking candidate could more eas-
ily win the next election. We found that this approach
created subtle issues around availability (a lower-ranked
server might need to time out and become a candidate
again if a higher-ranked server fails, but if it does so too
soon, it can reset progress towards electing a leader). We
made adjustments to the algorithm several times, but after
each adjustment new corner cases appeared. Eventually
we concluded that the randomized retry approach is more
obvious and understandable.

5.3 ログ複製

リーダーが選出された後は、クライアントの要求に対してサービスを開始する。各クライアントのリクエストは複製されたステートマシンによって実行されるコマンドを内包している。リーダーはそのコマンドを自信のログに新たなエントリーとして追加し、そのエントリーを複製するために AppendEntries RPC を他のサーバへパラレルに発行する。そのエントリー安全に複製されたら、リーダーはそのエントリーをステートマシンに適用し、クライアントへ実行結果を返す。もしフォロワーにクラッシュ、遅延、ネットワークパケットのロストが起こると、リーダーはフォロワーが全てのログログエントリーを保存するまで無限に（たとえそれがクライアントへ返事をした後であっても）AppendEntries RPC をリトライする。
 図6に示すように、ログは編成される。リーダーがエントリーを受信したとき各ログエントリは期番号を伴うステートマシンのコマンドを格納する。ログエントリー内の期番号はログ間の矛盾を検出するために、また、図3の幾つかの属性を満足させるために使われる。また、各ログエントリーはログ内での位置を識別する整数のインデックスを持つ。

5.4 安全性

前節ではRaftがリーダーを選出し、ログエントリーを複製する方法を述べた。しかし、これまでに述べたメカニズムでは、各ステートマシンが正確に同じコマンドを同じ順序で実行することを保障できない。例えば、フォロワーはリーダーがいくつかのログエントリーをコミットする間、止まっているかもしれず、そのような状況でリーダーが選出され、それらエントリーが新しいもので上書きされうる。その結果、異なるステートマシンが異なるコマンド列を実行するかもしれない。

この節では、リーダーに選出されることが許されるサーバを制限することでRaftアルゴリズムを完成させる。 この制限は、任意の任期のリーダーがそれ以前の任期におけるコミットの全エントリを持っていることを保証する。(図3 の Leader Completeness 属性を参照） 選挙に制約を与え、約束(コミットメント)のためのルールをより正確にする。最後に、リーダーの完全性属性に対する証明の絵を示し、それが複製されたステートマシンの正しい振る舞いを導出する方法を示す。

5.4.1 選挙制約

リーダーベースのアルゴリズムでは、リーダーは最終的に全てのコミットされたログエントリを保存しなければならない。
Viewstamped Replication [22] のような いくつかの合意アルゴリズムでは、リーダーは それがコミットされた全てのエントリを初期に持たなくても選出されうる。これらのアルゴリズムは選挙プロセスの間か、その後の短期間に、新しいリーダーへ彼がまだ持っていないエントリを識別し、転送する追加的な仕組みを持つ。残念ながら、これはかなりの追加的な仕組みと複雑さをもたらす。Raftは、前の期間の全てのコミットされたエントリを、選挙の瞬間から、それらエントリをリーダーへ転送すること無く、新しいリーダが持つ より単純なアプローチを用いる。これはログエントリがリーダーからフォロワーへの一方向へのみ流れ、リーダーはログの中の既存エントリを決して上書きしないことを意味する。

 Raftはログにコミットされた全エントリが格納されるまで候補者が選挙に勝たないようにするために投票プロセスを用いる。候補者は選ばれるためにクラスタの多数派とコンタクトしなければならず、これはコミットされた各エントリがそれらサーバ群のの少なくとも1つに存在しなければならないことを意味する。
 もし候補者のログが少なくとも多数派の他のログと同等に最新なら、それはコミットされた全エントリを持つだろう。(「最新」の意味は後で正確に定義する) RequestVote RPC はこの制限を実装する、即ち そのRPCは候補者の持つログについての情報を含み、投票者は彼自身の持つログが候補者のものより新しいなら投票を拒否する。
 
 **Raftは2つのログの何れがより新しいかを、ログ内の最新エントリの index と term  とを比較することによって、決定する。
 もし複数のログが term の異なる最新エントリを持つ場合、より後の term のログがより最新のものになる。もし複数のログが同じ term で終わっているなら、より長いログが最新である。**

5.4.2 前期間のエントリのコミット

 5.3節で述べた通り、リーダーが現在期間の あるエントリのコミットを知るのは、
サーバ群の多数派にそれが保存された後である。
もしエントリのコミット前にリーダーがクラッシュしたら、将来のリーダーはそのエントリの複製処理を完了しようとする。しかし、リーダーは前期間のあるエントリがサーバ群の多数派に保存されたことを直ぐには結論できない。
 Figure 8 は古いログエントリがサーバ群の多数派に格納され、将来のリーダによって上書きされうる状況を描いている。
 Figure 8 にあるような問題を取り除くため、Raftは、複製を数えることによって、前期間のログエントリを決してコミットしない。
**リーダーの当期におけるログエントリーだけが、複製を数えることによってコミットされる。当期のエントリーがこの方法でコミットされた後は、先行する全エントリーは、ログ整合特性を理由に、間接的にコミットされる。**
リーダーが古いログエントリーを安全にコミットできる状況はいくつかある (例えば、そのエントリーが各サーバに保存されている場合など)。 しかし、Raftは単純性のためにより保守的なアプローチをとる。
リーダーが前期のエントリを複製する際、ログエントリーは元の期番号を保持していることにより、Raft はコミット規則に余分な複雑さがある。
他の合意アルゴリズムでは、新たなリーダーが(複数の)前期におけるエントリーを再複製するとき、新たな "期番号" でそれを行わなければならない。
Raft のアプローチでは、ログエントリーは同じ期番号を時とログを越えて維持するので、ログエントリーの推論がより簡単になる。
加えて、Raft において 新たなリーダーが送信する前期のログエントリーは、他のアルゴリズムより少ない。(他のアルゴリズムでは、前期のログエントリーをコミットする前に番号を付け直すので、冗長なログエントリーを送らなければならない。)

5.4.3 安全性引数

5.5 フォロワーと候補者のクラッシュ

5.6 タイミングと可用性

6 クラスタメンバーシップの変更

安全な設定変更を実現するには、2フェーズアプローチが必要になる。
Raftでは、クラスタは最初に joint consensus と呼ぶ過渡的な設定になる。
joint consensus がコミットされると、システムは新しい設定に移行する。
joint consensus は古い設定と新しい設定の両方を組み合わせる。

- ログエントリは新旧両設定の全てのサーバーに複製される。
- 両設定における任意のサーバは、リーダーとしての役割を果たすことができる。
- (選挙とエントリのコミットに対する)合意は新旧両設定に別々の多数派を必要とする。

joint consensus は安全性に妥協することなく、個々のサーバが 異なる時刻に旧設定から新設定へ移行することを許す。さらには、joint consensus はクラスタが設定変更を行っている最中でもクライアントからのリクエストに応え続けられるようにする。

クラスタの設定は複製ログ内の特別なエントリを用いて保存・伝達される。Figure 11 は設定変更プロセスを示している。リーダが C old から C new へ設定変更するリクエストを受け付けたら、リーダはその設定を joint consensus (図中 C old,new) 用にログエントリとして保存し、前述の機構を使ってそのエントリを複製する。特定のサーバーが新しい構成エントリをログに追加すると、その構成を今後のすべての決定に使用します（サーバは、エントリがコミットされているかどうかに関係なく、常にログ内の最新の構成を使用します）。
これは、リーダーが C old,new のルールを使用して、C old,new のログエントリがいつコミットされるかを決めることを意味します。
リーダーがクラッシュした場合、勝った候補が C old,new を受け取ったかどうかに応じて、C old または C old,new のいずれかで新しいリーダーを選択できます。
いずれにせよ、C new はこの期間中に一方的な決定を下すことはできない。

C old,new がコミットされると、C old も C new も他方の承認なしに決定を下すことはできず、Leader Completeteness プロパティは C old,new のログエントリを持つサーバのみを確実にリーダーに選出できるようにします。
これでリーダーが C new を記述したログエントリを作成してクラスタに複製することが安全になった。繰り返すが、この設定は各サーバでそれが認識されるとすぐに有効になる。
新しい設定が C new のルールの下でコミットされると、古い設定は無関係になり、新しい設定にないサーバはシャットダウンすることができる。
図11に示すように、C old と C new の両方が一方的な決定をすることができる時間はなく、これが安全性を保証する。

7 ログの圧縮

8 クライアントとの対話

9 実装と評価

9.1 理解性

9.2 正確性

9.3 性能

10 関連する研究

11 結論

12 謝辞

## 参照
