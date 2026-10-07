# CLAUDE.md - AI Agent Rules for AIathelJP

Welcome, AI Agent! This project is a next-generation Ragnarok Online server emulator derived from Auriga (rAthena family), focused on high-precision database integrity and AI-driven development.

## 🚨 CRITICAL RULES (絶対遵守ルール)

1. **Protect Database Integrity:**
   - `item_db.txt` is FULLY SURVEYED and COMPLETED by the Project Manager.
   - NEVER overwrite or modify `item_db.txt` structure unless explicitly instructed.

2. **Auriga Syntax vs rAthena Syntax:**
   - This project uses Auriga-style script syntax and internal logic.
   - When porting features from rAthena, ALWAYS adapt the syntax to match Auriga conventions. Do NOT blindly copy rAthena code.

3. **Incremental Changes Only:**
   - Make small, focused, and single-purpose modifications.
   - Do not perform mass refactoring across multiple files at once.

## 🛠️ Project Environment

- **Base Engine:** Auriga (rAthena Branch)
- **Primary Language:** C / C++ (Server Core), Text/JSON/YAML (Database & Scripts)
- **Target Locale:** Japanese (UTF-8 / Shift-JIS depending on client requirements)

## 🎯 How to Assist the Manager

- Always ask for confirmation before modifying core socket/packet logic.
- When generating code, explain *where* from rAthena the feature originated and *how* it was converted to Auriga style.
- Keep comments in Japanese to align with the community.