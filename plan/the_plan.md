# Passtore Roadmap

Target: move from prototype to a local-first password manager that is secure, durable, and fast for daily secret retrieval.

## 1. Memory security and durability (highest priority)

### Milestone 1.1: Secure memory lifecycle
1. Add secure buffers for decrypted secrets in core and UI paths.
2. Zero memory on every boundary: after draw, after copy, after edit commit/cancel, on window close.
3. Lock sensitive memory pages where possible (Windows VirtualLock; fallback behavior documented).
4. Remove accidental plaintext persistence in logs/errors.

Acceptance criteria:
1. No decrypted secret remains in memory longer than one UI interaction cycle.
2. Secret zeroization is covered by tests and verified in debug instrumentation.
3. No plaintext secret appears in logs under normal and error scenarios.

### Milestone 1.2: Storage durability and integrity
1. Implement missing DB operations (delete, swap/reorder) and make them transactional.
2. Enforce SQLite durability profile: WAL mode, synchronous FULL for critical writes.
3. Add schema versioning + migration runner.
4. Add crash-safe write paths and startup integrity checks.

Acceptance criteria:
1. Power-loss simulation does not corrupt vault.
2. All CRUD operations are persistent across restart.
3. Startup integrity check detects corruption and gives safe recovery path.

### Milestone 1.3: Backup and restore
1. One-click encrypted backup export.
2. Restore with validation before overwrite.
3. Optional periodic local backup reminders.

Acceptance criteria:
1. Backup-restore roundtrip is deterministic and tested.
2. User can recover vault from backup in under 2 minutes.

## 2. Easy user access with only local password (no 2FA)

### Milestone 2.1: Proper unlock experience
1. Replace hardcoded open flow with:
   1. Vault selection/creation.
   2. Password prompt.
   3. Clear invalid-password UX.
2. Keep local-only model; no network auth, no 2FA dependency.
3. Add optional “remember vault path” only (never remember password by default).

Acceptance criteria:
1. First-time user can create vault and unlock in one guided flow.
2. Returning user unlocks in under 5 seconds.
3. Wrong password feedback is clear and safe (no detail leakage).

### Milestone 2.2: Strong local password security
1. Use Argon2id (or scrypt if constrained) for key derivation with calibrated cost.
2. Per-vault random salt, versioned KDF params.
3. Re-key path for password change without data loss.

Acceptance criteria:
1. KDF parameters are configurable and migration-safe.
2. Password change works for existing vaults and passes migration tests.

### Milestone 2.3: Friction-balanced protections
1. Auto-lock on inactivity (default on).
2. Optional lock on minimize/suspend.
3. Clipboard auto-clear timer.

Acceptance criteria:
1. Unlock remains simple while unattended risk is reduced.
2. Clipboard is cleared reliably after timeout.

## 3. Easy access to a single secret (readable and pretty table)

### Milestone 3.1: Retrieval speed UX
1. Add always-visible search/filter bar in main window.
2. Keyboard-first flow:
   1. Focus search.
   2. Arrow select row.
   3. Copy login/password with shortcut.
3. Add dedicated actions: copy login, copy password, reveal/hide password.

Acceptance criteria:
1. Typical retrieval path takes 2-3 actions.
2. 10k-row vault remains responsive for search/filter.
3. Password stays hidden by default; reveal is explicit.

### Milestone 3.2: Readability and visual polish
1. Improve table density, spacing, and column sizing rules.
2. Add row highlighting and better status feedback for copy/reveal actions.
3. Consistent typography and color contrast (accessibility baseline).

Acceptance criteria:
1. Important columns remain readable at common window sizes.
2. UI feedback is immediate and unambiguous.
3. Visual consistency across settings states and dialogs.

### Milestone 3.3: Single-secret detail panel
1. Optional side panel or popover for selected entry details.
2. Keep list context while exposing full fields (notes/url/etc).

Acceptance criteria:
1. User can inspect one entry without losing search/list context.
2. No secret field is displayed unless selected/revealed intentionally.

## 4. Changeable settings with minimal default view

### Milestone 4.1: Working settings persistence
1. Wire settings dialog save path to persistent storage.
2. Persist per-column visibility, order, width, blur mode.
3. Apply settings on startup and live update table when changed.

Acceptance criteria:
1. Settings persist across restart.
2. Live preview reflects saved result accurately.

### Milestone 4.2: Minimal secure defaults (required)
Default first-run table:
1. Show: Name/Subject, Login.
2. Hide by default: Password value (masked), optional nonessential columns.
3. Keep quick reveal/copy for password.

Acceptance criteria:
1. Out-of-box user sees login quickly and password hidden.
2. User can opt into more fields in settings within one screen.

## 5. Other security measures (recommended)

### Milestone 5.1: Defensive controls
1. Brute-force throttling on repeated failed unlock attempts.
2. Constant-time comparisons where relevant.
3. Safe error handling that avoids revealing internals.

Acceptance criteria:
1. Repeated bad attempts are slowed without harming normal UX.
2. No sensitive internals leak through UI messages.

### Milestone 5.2: Data-handling hardening
1. Secure temp/clipboard policy (auto-clear + warnings on long retention).
2. Disable unintended secret exposure through tooltips/title bars.
3. Optional privacy mode to blur sensitive UI when app loses focus.

Acceptance criteria:
1. Secret leakage surfaces are reduced and testable.
2. Privacy mode behavior is predictable and user-controlled.

### Milestone 5.3: Quality and verification
1. Add unit/integration tests for encryption lifecycle, CRUD durability, settings persistence, and search/copy/reveal flows.
2. Add static analysis and CI gates for regressions.

Acceptance criteria:
1. Security-critical paths have automated test coverage.
2. Release build requires all security and durability checks green.

## Suggested delivery plan (8 weeks)
1. Weeks 1-2: Memory lifecycle + durability foundation (Section 1).
2. Weeks 3-4: Unlock flow + KDF + local-password hardening (Section 2).
3. Weeks 5-6: Search/copy/reveal + readable/polished table UX (Section 3).
4. Week 7: Settings persistence + secure minimal defaults (Section 4).
5. Week 8: Extra controls, audits, CI/security test gates (Section 5).

## Release readiness checklist
1. No hardcoded password/path in startup flow.
2. Password hidden by default; login visible by default.
3. Search and single-action copy of login/password.
4. Settings persist and apply at startup.
5. Delete/reorder fully implemented and durable.
6. Backup/restore roundtrip verified.
7. Auto-lock + clipboard clear enabled by default.
8. Security and durability tests pass in CI.
