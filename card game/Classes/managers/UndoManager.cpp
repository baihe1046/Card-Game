#include "UndoManager.h"

void UndoManager::pushRecord(const OperationRecord& record) {
    _undoStack.push(record);
}

OperationRecord UndoManager::popRecord() {
    if (_undoStack.empty()) {
        throw std::runtime_error("Undo stack is empty");
    }

    OperationRecord record = _undoStack.top();
    _undoStack.pop();
    return record;
}

bool UndoManager::isEmpty() const {
    return _undoStack.empty();
}

void UndoManager::clear() {
    while (!_undoStack.empty()) {
        _undoStack.pop();
    }
}