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

<strong>Release goal</strong>
- Complete UCI communication
- Double check all writes to stdout/stderr and make sure that only UCI commands go to stdout

<strong>GUI to Engine Communications</strong>
- [ x ] uci
- [ x ] debug
- [ x ] isready
- [ x ] setoption
- [ ? ] register
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
- [ ? ] copyprotection
- [ ? ] registration
- [ - ] info
    - [ - ] depth
    - [ - ] seldepth
    - [ - ] time
    - [ - ] nodes
    - [ - ] pv
    - [ - ] multipv
    - [ - ] score
        - [ - ] cp
        - [ - ] mate
        - [ - ] lowerbound
        - [ - ] upperbound
    - [ - ] currmove
    - [ - ] currmovenumber
    - [ - ] hashfull
    - [ - ] nps
    - [ - ] tbhits
    - [ - ] sbhits
    - [ - ] cpuload
    - [ - ] string
    - [ - ] refutation
    - [ - ] currline
- [ - ] option
    - [ - ] name
        - [ - ] Hash
        - [ - ] NalimovPath
        - [ - ] NalimovCache
        - [ - ] Ponder
        - [ - ] OwnBook
        - [ - ] MultiPV
        - [ - ] UCI_ShowCurrLine
        - [ - ] UCI_ShowRefutations
        - [ - ] UCI_LimitStrength
        - [ - ] UCI_Elo
        - [ - ] UCI_AnalyseMode
        - [ - ] UCI_Opponent
        - [ - ] UCI_EngineAbout
        - [ - ] UCI_ShredderbasesPath
        - [ - ] UCI_SetPositionValue
    - [ - ] type
        - [ - ] check
        - [ - ] spin
        - [ - ] combo
        - [ - ] button
        - [ - ] string
    - [ - ] default
    - [ - ] min
    - [ - ] max
    - [ - ] var