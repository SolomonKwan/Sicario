# Information
This file is just for me to document project development information. Purely for my own information.

<strong>Potential Considerations</strong>
- Precompute and store all the moves as integers at compile time?

<strong>Upcoming release</strong>
</br>
v0.9.0

<strong>Branches of interest</strong>
- release/0.9.0
- feature/uci
- feature/mcts

<strong>Release goal</strong>
- Complete UCI communication
- Double check all writes to stdout/stderr and make sure that only UCI commands go to stdout
- Rewrite and cleanup the basic mcts algorithm

<strong>GUI to Engine Communications</strong>
- [ x ] uci
- [ x ] debug
- [ x ] isready
- [ x ] setoption
- [ ? ] register (what is this???)
- [ x ] ucinewgame
- [ x ] position
- [ x ] go
    - [ x ] searchmoves
    - [ x ] ponder
    - [ x ] wtime
    - [ x ] btime
    - [ x ] winc
    - [ x ] binc
    - [ x ] movestogo
    - [ x ] depth
    - [ x ] nodes
    - [ x ] mate
    - [ x ] movetime
    - [ x ] infinite
- [ x ] stop
- [ x ] ponderhit
- [ x ] quit

<strong>Engine to GUI Communications</strong>
- [ x ] id
    - [ x ] name
    - [ x ] author
- [ x ] uciok
- [ x ] readyok
- [ x ] bestmove
- [ ? ] copyprotection (not sure what to put this. not important. do another time.)
- [ ? ] registration (ditto)
- [ x ] info
    - [ x ] depth
    - [ x ] seldepth
    - [ x ] time
    - [ x ] nodes
    - [ x ] pv
    - [ x ] multipv
    - [ x ] score
        - [ x ] cp
        - [ x ] mate
        - [ x ] lowerbound
        - [ x ] upperbound
    - [ x ] currmove
    - [ x ] currmovenumber
    - [ x ] hashfull
    - [ x ] nps
    - [ ? ] tbhits (not using table bases so not relevant)
    - [ ? ] sbhits (ditto)
    - [ ? ] cpuload (eh, not that relevant. maybe include later if interested for whatever reason)
    - [ ? ] string (dont see a need for this)
    - [ ? ] refutation (refutation is kind of redundant. bestmove or the pvs somehwat do this already)
    - [ ? ] currline (too much. cant be bothered and somewhat redundant as well)
- [ x ] option
    - [ x ] name
        - [ x ] Hash
        - [ ? ] NalimovPath (dont want to use precomputed moves)
        - [ ? ] NalimovCache (dont want to use precomputed moves)
        - [ x ] Ponder
        - [ ? ] OwnBook (dont want to use precomputed moves)
        - [ x ] MultiPV
        - [ ? ] UCI_ShowCurrLine (somewhat redundant)
        - [ x ] UCI_ShowRefutations
        - [ x ] UCI_LimitStrength
        - [ x ] UCI_Elo
        - [ x ] UCI_AnalyseMode
        - [ x ] UCI_Opponent
        - [ ? ] UCI_EngineAbout (not important/relevant. already sends engine info at program start)
        - [ ? ] UCI_ShredderbasesPath (dont want to use precomputed moves)
        - [ ? ] UCI_SetPositionValue (dont see a use for this for sicario)
    - [ x ] type
        - [ x ] check
        - [ x ] spin
        - [ x ] combo
        - [ x ] button
        - [ x ] string
    - [ x ] default
    - [ x ] min
    - [ x ] max
    - [ x ] var