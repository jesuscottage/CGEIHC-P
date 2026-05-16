# Machote — Claude Code Project Template

Reusable repository template for organizing any project with Claude Code
in the most effective way possible.

## What's included

- **51 skills** organized by category (development, research, leads, YouTube, infrastructure)
- **4 shared rules** (organization, language, security, quality)
- **1 research agent** with knowledge base persistence
- **1 semantic commit command** (bilingual)
- **Knowledge management** with mandatory indexes
- **MCP catalog** (18+ recommended servers, not installed by default)

## Quick setup

### 1. Clone

```bash
git clone https://github.com/jesuscottage/machote.git my-project
cd my-project
rm -rf .git
git init
```

### 2. Activate MCPs (optional)

```bash
cp .mcp.json.example .mcp.json
# Edit .mcp.json: uncomment the servers you need
```

See `setup/mcp-catalog.md` for the complete catalog with descriptions.

### 3. Customize

1. Edit `CLAUDE.md` with your project's info (name, stack, structure)
2. Edit `.claude/context/reminders.md` with your specific context
3. Edit `.claude/rules/idioma.md`: change `idioma-principal` if needed (default: `es`)
4. Add business rules in `.claude/rules/negocio.md` (create new file)
5. Populate `docs/knowledge/` with your research

### 4. Open Claude Code

**Always open from the project root.** The rules only load from the root.

---

## Adapt to an existing project

If you already have a project and want to adopt this organization, ask Claude:

```
Adapt the machote organization to this project.
The machote is at [path-to-cloned-machote].
Adapt the rules and skills to this stack/context.
```

### Safety guarantees

The adaptation is **strictly additive** — it will NOT break your existing project:

- **Only creates** new folders (`.claude/`, `docs/knowledge/`, `docs/plans/`, `setup/`)
- **Never touches** your code, configs, package.json, Docker, CI/CD, APIs, or .env files
- **Never moves** existing project files — if you already have skills outside `.claude/`, they get copied (not moved) until you confirm
- **Never modifies** API connections, credentials, deploy configuration, or dependencies
- If `CLAUDE.md` already exists, Claude **appends** to it (doesn't replace)
- If `.mcp.json` already exists, Claude **doesn't touch it** (only creates `.mcp.json.example` as reference)

### Adaptation checklist

Claude follows this checklist when adapting:

1. Identify what already exists: `ls .claude/ docs/ setup/ 2>/dev/null`
2. List potential conflicts (files that already exist at target paths)
3. Present the list of proposed changes BEFORE executing
4. Wait for explicit user confirmation
5. Create backup of any file that will be modified
6. Execute additive changes only
7. Verify the project still works (`git status` — no original project files were modified)

### Step by step

```
To adapt this machote to your existing project:

1. Open Claude Code at your project root
2. Tell Claude: "Adapt the machote organization to this project.
   The machote is at [path-to-cloned-machote]."
3. Claude will read the machote's CLAUDE.md and show you
   exactly which folders/files it will create
4. Confirm the changes
5. Claude will create the structure WITHOUT touching your code or configs

Safety guarantees:
- Your code, APIs, .env, Docker, CI/CD are NOT touched
- Only Claude organization folders are created
- If you already have skills elsewhere, they are copied (not moved)
```

---

## Featured skills

### For ANY development project
- `/prompt-contract` — Contract before implementing (prevents rework)
- `/agent-review` — Automatic review post-implementation
- `/reverse-prompt` — Clarifying questions (prevents assumptions)
- `/revision-sistematica` — Exhaustive academic research (7 phases, 6 APIs)
- `/consultar-conocimiento` — Query your knowledge base
- `/generar-informe` — Project status report
- `/stochastic-multi-agent-consensus` — Poll N agents for decisions
- `/model-chat` — Debate between Claude instances

### For agencies / freelancers
- Gmail, leads, Instantly, proposals, onboarding skills
- YouTube skills (outliers, thumbnails, metadata, tracking)
- `/design-website` — HTML mockups for prospects

### For complex decisions
- `/stochastic-multi-agent-consensus` — Poll of N agents with consensus aggregation
- `/model-chat` — Multi-instance debate with synthesis
- `/consultar-gemini` — Architecture consultations with Gemini as second brain

---

## Claude's persistent memory

Claude automatically stores persistent memory in:
```
~/.claude/projects/{project-hash}/memory/
```

Recommended organization:
- `MEMORY.md` — Memory index
- `feedback_*.md` — User feedback on Claude's behavior
- `project_*.md` — Project information (stack, state, decisions)
- `reference_*.md` — External references (repos, URLs, contacts)

This is automatic — you don't need to create it manually.

---

## Project structure explained

| Directory | Purpose |
|-----------|---------|
| `.claude/rules/` | Shared rules that Claude must follow |
| `.claude/skills/` | ALL skills — one folder per skill with SKILL.md |
| `.claude/agents/` | Agent definitions (researcher, etc.) |
| `.claude/commands/` | Custom slash commands (/commit, etc.) |
| `.claude/context/` | Context auto-injected at session start |
| `docs/knowledge/` | Persistent knowledge base with INDEX.md |
| `docs/plans/` | Actionable plans with progress tracking |
| `docs/reviews/` | Code reviews and consultations (created as needed) |
| `scripts/` | Auxiliary scripts (orthography fix, etc.) |
| `setup/` | Setup instructions (MCPs) |

---

## License

MIT
