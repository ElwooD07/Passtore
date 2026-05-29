# Known Security Vulnerabilities & Risks

## VUL-1 — ResourceDefinitions stored in plaintext

**Severity**: Low–Medium  
**Introduced**: Phase 2 (step 9) — `ResourceDefinitions` table created in `BuildOpenedDb`.

### Description
The encrypted `Resources` table reveals nothing without the password. However, the `ResourceDefinitions` table (column names, `big` flag, sort order) is stored as unencrypted SQLite data in the same `.db` file. Anyone with filesystem access to the `.db` file can open it in any SQLite viewer and read the schema.

For the default column set (Name, URL, Login, Password, Notes) this is low impact — the schema is not secret. The risk rises if users ever define custom columns with sensitive names (e.g. `"Bitcoin seed phrase"`, `"SSN"`).

### Mitigation options
- Encrypt the `ResourceDefinitions` rows using the same key as `Resources` (store as encrypted blobs).
- Accept the risk for the default schema; document that column names should not contain sensitive information.

### Status
Accepted for now. No user-defined columns exist (dynamic columns deferred). Revisit before dynamic column support is added.

---

## VUL-2 — TableSettings JSON is unauthenticated

**Severity**: Low  
**Introduced**: Phase 2 (steps 5–8) — `Settings::Load` / `Settings::Save` write a plaintext JSON file next to the `.db`.

### Description
The settings file is not signed or MAC'd. An attacker with write access to the settings directory can tamper with column visibility or `blured` flags — for example, setting `"blured": false` on the Password column so that passwords are displayed in plaintext in the table the next time the app opens.

`MergeWithSaved` correctly drops any column name from the JSON that does not exist in the DB definition, so there is no column injection vector. But the display-protection bypass (un-blurring) is real.

### Mitigation options
- Sign the settings file with an HMAC keyed on the master key (requires the DB to be open before settings can be validated).
- Store the table settings inside the encrypted DB rather than in a sidecar JSON file.

### Status
Accepted for now. The settings file controls UI presentation only — no data is exposed beyond what the attacker could read anyway if they have write access to the directory. Revisit if settings grow to include anything security-critical.

---

## Deferred security work (tracked elsewhere)

- **Argon2id**: KDF is currently PBKDF2-SHA256 (100 000 iterations). Argon2id deferred to a dedicated security hardening sprint.
- **Password chain**: `ChangePassword` drops and recreates the `Metadata` table, losing history. Deferred.
- **Old-password verification in `ChangePassword`**: currently a TODO — the old password is not checked before re-encrypting keys.
