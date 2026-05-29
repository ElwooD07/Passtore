# Passtore Sprint Plan — Usable Local Product

## TL;DR
Three areas need work to make Passtore a usable product: CRUD is broken (delete/swap are stubs), settings are not persisted, and UI needs polish. A fourth track produces a written design decision for cloud extensibility. Dynamic column management has been deferred — there is no concrete user story that justifies it today.

---

## Phase 1 — Fix Broken CRUD (unblocks everything else)

1. **Implement `DeleteResource()`** in `src/PasstoreLib/Storages/SQLite/SQLiteDatabase.cpp` — execute `DELETE FROM Resources WHERE ROWID=?`. Without this, the Delete button in the UI silently does nothing.
2. **Implement `Swap()`** — exchange the `Data` blobs of two ROWIDs inside a transaction. This unblocks row reordering.
3. **Fix `Cache<K,T>`** in `src/Passtore/Models/Cache.h` — the comment says "so strange, reimplement it". The `m_lastUsed` map is keyed on timestamp, not on access order, making LRU eviction unreliable. Replace with a standard ordered-map or doubly-linked-list LRU.
4. **Verify Delete flows end-to-end** — `ResourcesListWidget::onDelete` → `ResourceTableModel::deleteRow` → `IResourceStorage::DeleteResource` → DB. Add a test in PasstoreLibTest.

**Verification**: Run PasstoreLibTest; add/delete/swap rows in the running app and confirm persistence across restarts.

---

## Phase 2 — Settings Persistence

5. **Design config file format** — use JSON (SimpleJSON already available), stored in the same directory as the `.db` file. Holds: column visibility, column blur flags, and (later) user-defined column definitions.
6. **Implement `Settings` load/save** — `Settings.cpp` is empty. Add `Settings::Load(path)` and `Settings::Save(path)` using SimpleJSON. Define the schema now so Phase 3 can extend it without a breaking change.
7. **Wire `SettingsDialog::onSave()`** — currently just calls `hide()`. Make it call `Settings::Save()` and propagate `TableSettings` back to `MainWindow` → `ResourcesListWidget::refresh()`.
8. **Apply settings at startup** — `MainWindow` must load `Settings` after `Open()` and pass column visibility/blur to the table model.
9. **Persist `ResourcesDefinition` to DB** — `SQLiteDatabase::GetResourcesDefinition()` is hardcoded. Create a `ResourceDefinitions` table in `BuildOpenedDb()`. Load from it in `GetResourcesDefinition()`, migrate existing DBs by inserting defaults if the table is absent.

**Verification**: Change column visibility, close app, reopen — settings survive.

---

## Phase 3 — UI Polish

13. **FLTK color scheme** — call `Fl::set_color()` / `Fl::background()` once at startup in `main.cpp`. Neutral palette; monospace only for password/critical fields.
14. **Font consistency** — `FL_HELVETICA` / `FL_HELVETICA_BOLD` globally; `FL_COURIER` for big/critical cell editors.
15. **Table row height and column widths** — replace magic numbers in `ResourcesListWidget` with values proportional to font size. Add minimum column widths.
16. **Edit UX** — inline editing stays; a detail side panel for multi-line Notes is a stretch goal.
17. **Password change dialog** — `ChangePassword()` exists on `IResourceStorage` but has no UI entry point and old-password verification is a TODO. Add a menu item and a 3-field dialog (old password, new password, confirm).

**Verification**: Visual review; no regressions in tests.

---

## Phase 4 — S3 Extensibility Analysis (design only, no code)

18. **Gap analysis** — `IResourceStorage` is sync-only and ROWID-ordered. Specific gaps for any cloud backend:
    - No `ListResources()` / `GetPage()` for paginated/lazy loading.
    - No `BeginSync()` / `EndSync()` concept for eventual-consistency backends.
    - `GetNext()` iterates by local ROWID — meaningless on S3.
    - No authentication/session lifecycle beyond `Open()`.
    - Key material (salt + encrypted phrase + encrypted master key) is already self-contained per record — this is fine and carries forward.
19. **Proposed interface additions** — document (not implement): `GetPage(offset, count)`, `SyncAll()`, async variant via callback or future.
20. **Write design note** — `plan/s3-backend.md` describing the above gaps and proposed additions.

**Verification**: Document present in `plan/`.

---

## Step Dependencies

```
Phase 1 → Phase 2
Phases 1–3 run in parallel to Phase 4
```

---

## Relevant Files

- `src/PasstoreLib/Storages/SQLite/SQLiteDatabase.cpp` — DeleteResource, Swap, GetResourcesDefinition stubs
- `src/PasstoreLib/Storages/IResourceStorage.h` — consumer interface; schema management is not part of it
- `src/Passtore/Models/Cache.h` — buggy LRU, needs rewrite
- `src/Passtore/Settings.h` / `Settings.cpp` — empty persistence layer
- `src/Passtore/Widgets/SettingsDialog.cpp` — onSave() is a no-op
- `src/Passtore/Widgets/MainWindow.cpp` — startup settings load goes here
- `src/Passtore/Widgets/ResourcesListWidget.cpp` — UI polish target
- `src/Passtore/main.cpp` — global FLTK theme goes here
- `plan/` — S3 design note target

## Decisions

- **Dynamic columns**: deferred — no concrete user story justifies a schema editor. The fixed set (Name, URL, Login, Password, Notes) covers all current use cases. `GetResourcesDefinition()` returning persisted definitions is sufficient; `IResourceStorage` is not extended for writes.
- **Config file**: JSON in same directory as the `.db` file.
- **Edit UX**: inline stays; detail panel is stretch.
- **S3 backend**: analysis only this sprint, no implementation.
- **Argon2id / password-chain**: deferred to a security hardening sprint.
