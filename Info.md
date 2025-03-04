# Information
This file is just for me to document project development information. Purely for my own information.

<strong>Upcoming release</strong>
</br>
v0.9.0

<strong>Release goal</strong>
- Complete UCI communication

<strong>GUI to Engine Communications</strong>
- [ - ] uci
- [ - ] debug
- [ - ] isready
- [ - ] setoption
- [ - ] register
- [ - ] ucinewgame
- [ - ] position
- [ - ] go
    - [ - ] searchmoves
    - [ - ] ponder
    - [ - ] wtime
    - [ - ] btime
    - [ - ] winc
    - [ - ] binc
    - [ - ] movestogo
    - [ - ] depth
    - [ - ] nodes
    - [ - ] mate
    - [ - ] movetime
    - [ - ] infinite
- [ - ] stop
- [ - ] ponderhit
- [ - ] quit

<strong>Engine to GUI Communications</strong>
- [ - ] id
    - [ - ] name
    - [ - ] author
- [ - ] uciok
- [ - ] readyok
- [ - ] bestmove
- [ - ] copyprotection
- [ - ] registration
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