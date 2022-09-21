#include "evaluate.hpp"

template<>
float Evaluator::psqtEvaluation<OPENING>(Position& position) {
    float value = 0;

    value += PSQT::Opening::KING[position.getPieceSquare<W_KING>(0)];
    value -= PSQT::Opening::KING[reflectVertical(position.getPieceSquare<B_KING>(0))];

    for (uint i = 0; i < position.getPieceIndex<W_QUEEN>(); i++)
        value += PSQT::Opening::QUEEN[position.getPieceSquare<W_QUEEN>(i)];
    for (uint i = 0; i < position.getPieceIndex<B_QUEEN>(); i++)
        value -= PSQT::Opening::QUEEN[reflectVertical(position.getPieceSquare<B_QUEEN>(i))];

    for (uint i = 0; i < position.getPieceIndex<W_ROOK>(); i++)
        value += PSQT::Opening::ROOK[position.getPieceSquare<W_ROOK>(i)];
    for (uint i = 0; i < position.getPieceIndex<B_ROOK>(); i++)
        value -= PSQT::Opening::ROOK[reflectVertical(position.getPieceSquare<B_ROOK>(i))];

    for (uint i = 0; i < position.getPieceIndex<W_BISHOP>(); i++)
        value += PSQT::Opening::BISHOP[position.getPieceSquare<W_BISHOP>(i)];
    for (uint i = 0; i < position.getPieceIndex<B_BISHOP>(); i++)
        value -= PSQT::Opening::BISHOP[reflectVertical(position.getPieceSquare<B_BISHOP>(i))];

    for (uint i = 0; i < position.getPieceIndex<W_KNIGHT>(); i++)
        value += PSQT::Opening::KNIGHT[position.getPieceSquare<W_KNIGHT>(i)];
    for (uint i = 0; i < position.getPieceIndex<B_KNIGHT>(); i++)
        value -= PSQT::Opening::KNIGHT[reflectVertical(position.getPieceSquare<B_KNIGHT>(i))];

    for (uint i = 0; i < position.getPieceIndex<W_PAWN>(); i++)
        value += PSQT::Opening::PAWN[position.getPieceSquare<W_PAWN>(i)];
    for (uint i = 0; i < position.getPieceIndex<B_PAWN>(); i++)
        value -= PSQT::Opening::PAWN[reflectVertical(position.getPieceSquare<B_PAWN>(i))];

    return value;
}

template<>
float Evaluator::psqtEvaluation<MIDDLEGAME>(Position& position) {
    float value = 0;

    value += PSQT::MiddleGame::KING[position.getPieceSquare<W_KING>(0)];
    value -= PSQT::MiddleGame::KING[reflectVertical(position.getPieceSquare<B_KING>(0))];

    for (uint i = 0; i < position.getPieceIndex<W_QUEEN>(); i++)
        value += PSQT::MiddleGame::QUEEN[position.getPieceSquare<W_QUEEN>(i)];
    for (uint i = 0; i < position.getPieceIndex<B_QUEEN>(); i++)
        value -= PSQT::MiddleGame::QUEEN[reflectVertical(position.getPieceSquare<B_QUEEN>(i))];

    for (uint i = 0; i < position.getPieceIndex<W_ROOK>(); i++)
        value += PSQT::MiddleGame::ROOK[position.getPieceSquare<W_ROOK>(i)];
    for (uint i = 0; i < position.getPieceIndex<B_ROOK>(); i++)
        value -= PSQT::MiddleGame::ROOK[reflectVertical(position.getPieceSquare<B_ROOK>(i))];

    for (uint i = 0; i < position.getPieceIndex<W_BISHOP>(); i++)
        value += PSQT::MiddleGame::BISHOP[position.getPieceSquare<W_BISHOP>(i)];
    for (uint i = 0; i < position.getPieceIndex<B_BISHOP>(); i++)
        value -= PSQT::MiddleGame::BISHOP[reflectVertical(position.getPieceSquare<B_BISHOP>(i))];

    for (uint i = 0; i < position.getPieceIndex<W_KNIGHT>(); i++)
        value += PSQT::MiddleGame::KNIGHT[position.getPieceSquare<W_KNIGHT>(i)];
    for (uint i = 0; i < position.getPieceIndex<B_KNIGHT>(); i++)
        value -= PSQT::MiddleGame::KNIGHT[reflectVertical(position.getPieceSquare<B_KNIGHT>(i))];

    for (uint i = 0; i < position.getPieceIndex<W_PAWN>(); i++)
        value += PSQT::MiddleGame::PAWN[position.getPieceSquare<W_PAWN>(i)];
    for (uint i = 0; i < position.getPieceIndex<B_PAWN>(); i++)
        value -= PSQT::MiddleGame::PAWN[reflectVertical(position.getPieceSquare<B_PAWN>(i))];

    return value;
}

template<>
float Evaluator::psqtEvaluation<ENDGAME>(Position& position) {
    float value = 0;

    value += PSQT::EndGame::KING[position.getPieceSquare<W_KING>(0)];
    value -= PSQT::EndGame::KING[reflectVertical(position.getPieceSquare<B_KING>(0))];

    for (uint i = 0; i < position.getPieceIndex<W_QUEEN>(); i++)
        value += PSQT::EndGame::QUEEN[position.getPieceSquare<W_QUEEN>(i)];
    for (uint i = 0; i < position.getPieceIndex<B_QUEEN>(); i++)
        value -= PSQT::EndGame::QUEEN[reflectVertical(position.getPieceSquare<B_QUEEN>(i))];

    for (uint i = 0; i < position.getPieceIndex<W_ROOK>(); i++)
        value += PSQT::EndGame::ROOK[position.getPieceSquare<W_ROOK>(i)];
    for (uint i = 0; i < position.getPieceIndex<B_ROOK>(); i++)
        value -= PSQT::EndGame::ROOK[reflectVertical(position.getPieceSquare<B_ROOK>(i))];

    for (uint i = 0; i < position.getPieceIndex<W_BISHOP>(); i++)
        value += PSQT::EndGame::BISHOP[position.getPieceSquare<W_BISHOP>(i)];
    for (uint i = 0; i < position.getPieceIndex<B_BISHOP>(); i++)
        value -= PSQT::EndGame::BISHOP[reflectVertical(position.getPieceSquare<B_BISHOP>(i))];

    for (uint i = 0; i < position.getPieceIndex<W_KNIGHT>(); i++)
        value += PSQT::EndGame::KNIGHT[position.getPieceSquare<W_KNIGHT>(i)];
    for (uint i = 0; i < position.getPieceIndex<B_KNIGHT>(); i++)
        value -= PSQT::EndGame::KNIGHT[reflectVertical(position.getPieceSquare<B_KNIGHT>(i))];

    for (uint i = 0; i < position.getPieceIndex<W_PAWN>(); i++)
        value += PSQT::EndGame::PAWN[position.getPieceSquare<W_PAWN>(i)];
    for (uint i = 0; i < position.getPieceIndex<B_PAWN>(); i++)
        value -= PSQT::EndGame::PAWN[reflectVertical(position.getPieceSquare<B_PAWN>(i))];

    return value;
}

float Evaluator::psqtEvaluation(Position& position) {
    switch (Evaluator::getStage(position)) {
        case MIDDLEGAME:
            return psqtEvaluation<MIDDLEGAME>(position);
        case ENDGAME:
            return psqtEvaluation<ENDGAME>(position);
        default: // OPENING
            return psqtEvaluation<OPENING>(position);
    }
}

float Evaluator::getPieceEvaluation(Position& position) {
    float value = 0;
    value += position.getPieceIndex<W_QUEEN>() * PieceValue::QUEEN;
    value += position.getPieceIndex<W_ROOK>() * PieceValue::ROOK;
    value += position.getPieceIndex<W_BISHOP>() * PieceValue::BISHOP;
    value += position.getPieceIndex<W_KNIGHT>() * PieceValue::KNIGHT;
    value += position.getPieceIndex<W_PAWN>() * PieceValue::PAWN;

    value -= position.getPieceIndex<B_QUEEN>() * PieceValue::QUEEN;
    value -= position.getPieceIndex<B_ROOK>() * PieceValue::ROOK;
    value -= position.getPieceIndex<B_BISHOP>() * PieceValue::BISHOP;
    value -= position.getPieceIndex<B_KNIGHT>() * PieceValue::KNIGHT;
    value -= position.getPieceIndex<B_PAWN>() * PieceValue::PAWN;
    return value;
}

GameStage Evaluator::getStage(Position& position) {
    if (position.getCastling() != 0) {
        return OPENING;
    } else if (position.getQueenCount() == 0 && position.getPieceCnt() <= 20) {
        return ENDGAME;
    } else {
        return MIDDLEGAME;
    }
}