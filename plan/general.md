# Qt → FLTK Migration Plan

## Problem
Qt stores display data in QAbstractTableModel/QVariant — a known interception target for memory forensics.
Goal: remove Qt entirely, gain full control over sensitive data lifetime in memory.

## Decision: FLTK (not Nana)

### FLTK pros
- No data model layer — you own all buffers, control zeroing explicitly
- Thin rendering: draw callback → your buffer → display, no hidden copies
- 30+ years mature, predictable memory layout, small binary footprint
- No MOC, no hidden allocators, no string interning
- Fl_Table gives direct row draw control

### FLTK cons
- Dated default look (mitigable with custom themes)
- No built-in signal/slot — need observer/callback pattern (but that's the goal)
- No .ui designer files — layouts in code (actually desirable for security review)
- Fl_Table API is lower-level than QAbstractTableModel (more work, more control)

### Nana rejected because
- Sparse maintenance (last activity 2022)
- Uses shared_ptr and internal event queues — similar hidden allocation risk to Qt
- Much smaller community, fewer MSVC-tested builds

---

## Scope

### What changes
- `src/Passtore/` — all widgets, models, delegates, pch.h, main.cpp
- `src/PasstoreLib/Security/Cryptor.h/.cpp` — remove Qt overloads
- `src/PasstoreLib/Security/SensitiveData.h` — remove Qt section
- `CMakeLists.txt` — replace Qt6 with FLTK

### What stays
- `src/PasstoreLib/` core — already zero Qt dependencies (SQLite, Crypto, Marshaling)
- `ext/` — TinyAES, sqlite, plog, gtest untouched
- All data types: `Data`, `Resource`, `ResourceValue` — stay std::

---

## Phases

### Phase 1 — Strip Qt from PasstoreLib (trivial, no risk)
- Remove `#ifdef USE_QT` blocks from `Cryptor.h/.cpp` and `SensitiveData.h`
- Remove `#define USE_QT` from `PasstoreLib/pch.h`
- PasstoreLib must build with zero Qt headers

### Phase 2 — Add FLTK to build system
- Add `ext/fltk/` (static build from source)
- Update root `CMakeLists.txt`: remove Qt6, add FLTK
- Verify FLTK hello-world compiles

### Phase 3 — Replace data layer (no UI yet)
- `Settings.h`: `QString` → `std::string`, `QVector` → `std::vector`
- `Cache.h`: `QMap` → `std::map`, `quint64` → `uint64_t`
- `ResourceTableModelRoles.h`: `Qt::UserRole` → plain enum constant
- New `TableModel` abstract interface (no Qt) — defines `rowCount`, `columnCount`, `cellData`, `setCellData`

### Phase 4 — Rewrite model/delegate (hardest)
- `ResourceTableModel`: rewrite using new `TableModel` interface backed by `IResourceStorage`
  - Full control over when data is fetched, zeroed, released
  - No QVariant boxing — data stays as `std::string` until draw time
  - Edit tracking implemented manually
- `ResourceViewDelegate` → replaced by custom `Fl_Table` draw/event callbacks
  - `draw_cell()` for display (blur sensitive columns)
  - Inline editor (`Fl_Input` / `Fl_Multiline_Input`) for edit mode

### Phase 5 — Rewrite widgets
- `MainWindow` → `Fl_Window`, no `Q_OBJECT`, callbacks instead of signals/slots
- `ResourcesListWidget` → `Fl_Table` subclass with custom `draw_cell()`
- `ColumnSettingsWidget` → `Fl_Window` with `Fl_Input` + `Fl_Check_Button`
- `SettingsDialog` → `Fl_Window` with dynamic `Fl_Group` layout
- Error propagation: `std::function<void(std::string)>` callback registered on model

### Phase 6 — Memory hardening (the real goal)
- Add `VirtualLock()` / `mlock()` to `Data` type (prevent paging to disk)
- Zero `Fl_Input` buffers explicitly on close/clear
- Add zeroing destructor to any struct holding decrypted strings in UI layer
- Audit: no decrypted `std::string` should outlive its display scope

---

## Architecture (target)

```
IResourceStorage ──► ResourceTableModel (plain C++ class)
                          │
                    Fl_Table subclass
                    └─ draw_cell()   → reads from model, zeroes after draw
                    └─ handle()      → inline Fl_Input editor, zeroed on commit
```

No boxing. No role system. No hidden copies.

---

## Pros/Cons

| Gain | Cost |
|---|---|
| No Qt heap offsets to intercept | ~60-90h of work |
| Explicit buffer lifetime | Manual draw/layout code |
| Auditable — no MOC-generated code | Lose HiDPI/accessibility support |
| No Qt DLLs to deploy | Observer pattern from scratch |
| `VirtualLock` on sensitive buffers | FLTK's dated default look |
