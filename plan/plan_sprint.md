# Passtore Sprint Plan — Usable Local Product

## TL;DR
Three areas need work to make Passtore a usable product: CRUD is broken (delete/swap are stubs), settings are not persisted, and UI needs polish. A fourth track produces a written design decision for cloud extensibility. Dynamic column management has been deferred — there is no concrete user story that justifies it today.

---

## Phase 1 — Fix Broken CRUD ✅ DONE

1. ✅ **`DeleteResource()`** — implemented: `DELETE FROM Resources WHERE ROWID=?`.
2. ✅ **`Swap()`** — implemented: reads both blobs, exchanges them inside a transaction.
3. ✅ **`Cache<K,T>`** — rewritten as a proper doubly-linked-list LRU (`std::list` + `std::unordered_map`, `splice`-based promotion).
4. ✅ **Delete flows verified end-to-end** — `ResourcesTableWidget::onDelete` → `ResourceTableModel::DeleteRow` → `SQLiteDatabase::DeleteResource` → DB. Tests `DeleteResource_RemovesFromDB` and `DeleteResource_PersistsAcrossReopen` pass.

---

## Phase 2 — Settings Persistence

5. ✅ **JSON config format** — `passtore.json` stored alongside the `.db` file, using SimpleJSON.
6. ✅ **`Settings::Load/Save`** — implemented in `Settings.cpp`; delegates to `LoadTableSettings`/`SaveTableSettings` in `Marshaling.cpp`.
7. ⚠️ **`SettingsDialog::onSave()`** — `onSave` and `onClose` both just call `hide()`. Propagation and save happen in `MainWindow::onSettings()` after the dialog closes, so settings ARE persisted — but Save and Close are indistinguishable. Save should apply changes; Close should discard them.
8. ✅ **Settings applied at startup** — `MainWindow` calls `m_settings.Load(settingsPath, defs)` in the constructor before building the UI.
9. ✅ **`ResourceDefinitions` persisted to DB** — `GetResourcesDefinition()` reads from the `ResourceDefinitions` table seeded on first open. No migration per project policy.

**Remaining**: Fix Save vs Close semantics in `SettingsDialog` (item 7).

---

## Phase 3 — UI Polish ✅ DONE

13. ✅ **FLTK color scheme** — `Fl::scheme("gtk+")`, neutral gray palette set in `main.cpp`.
14. ✅ **Font consistency** — `FL_HELVETICA`/`FL_HELVETICA_BOLD` globally; `FL_COURIER` for big/critical cell editors.
15. ✅ **Table row height and column widths** — magic numbers replaced with `s_`-prefixed constants proportional to font size in `ResourcesTableWidget.cpp`.
16. ✅ **Edit UX** — inline editing in place; detail panel deferred as planned.
17. ✅ **Password change dialog** — menu item wired in `MainWindow`, `ChangePasswordDialog` fully implemented with old/new/confirm fields; `SQLiteDatabase::ChangePassword` implemented and tested.

---

## Phase 4 — S3 Extensibility Analysis (design only, no code)

18. **Gap analysis** — `IResourceStorage` is sync-only and ROWID-ordered. Specific gaps for any cloud backend:
    - No `ListResources()` / `GetPage()` for paginated/lazy loading.
    - No `BeginSync()` / `EndSync()` concept for eventual-consistency backends.
    - `GetNext()` iterates by local ROWID — meaningless on S3.
    - No authentication/session lifecycle beyond `Open()`.
    - Key material (salt, encrypted phrase, encrypted master key) lives in a separate `Metadata` table in the DB, not per-record — on S3 this would be a separate object (e.g. `passtore.meta`) read once at open time. This is fine and carries forward.
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
- `src/Passtore/Widgets/SettingsDialog.cpp` — onSave() needs Save-vs-Close semantics fixed (item 7)
- `src/Passtore/Widgets/ResourcesTableWidget.cpp` — UI polish target (was ResourcesListWidget)
- `src/Passtore/main.cpp` — global FLTK theme goes here
- `plan/` — S3 design note target

## Decisions

- **Dynamic columns**: deferred — no concrete user story justifies a schema editor. The fixed set (Name, URL, Login, Password, Notes) covers all current use cases. `GetResourcesDefinition()` returning persisted definitions is sufficient; `IResourceStorage` is not extended for writes.
- **Config file**: JSON in same directory as the `.db` file.
- **Edit UX**: inline stays; detail panel is stretch.
- **S3 backend**: analysis only this sprint, no implementation.
- **Argon2id / password-chain**: deferred to a security hardening sprint.
