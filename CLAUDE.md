# CLAUDE.md — Machote (Claude Code Project Template)

> Reusable template for organizing any project with Claude Code.
> Primary language: configurable in `.claude/rules/idioma.md` (default: Spanish).
> See `README.md` for setup instructions.

## Project Structure

```
Machote/
├── CLAUDE.md                     # This file — project instructions for Claude
├── README.md                     # Setup and usage instructions
├── .gitignore                    # Ignored files
├── .mcp.json.example             # MCP catalog (not active by default)
├── .claude/
│   ├── rules/                    # Shared rules
│   │   ├── organizacion.md       # MANDATORY: directory structure
│   │   ├── idioma.md             # Language + Spanish accents
│   │   ├── seguridad.md          # OWASP, secrets, inputs
│   │   └── calidad.md            # Conventions, testing, docs
│   ├── agents/
│   │   └── investigador.md       # Research agent
│   ├── skills/                   # 51 skills organized by category
│   ├── commands/
│   │   └── commit.md             # Semantic commit + push + PR
│   └── context/
│       └── reminders.md          # Auto-injected at session start
├── scripts/
│   └── fix-spanish-ortho.sh      # Spanish orthography fix script
├── docs/
│   ├── knowledge/
│   │   └── INDEX.md              # Knowledge base index (mandatory)
│   └── plans/                    # Actionable plans with checkboxes
└── setup/
    └── mcp-catalog.md            # Exhaustive MCP catalog (18+)
```

## Rules

| Rule | File | What it defines |
|------|------|-----------------|
| Organization | `.claude/rules/organizacion.md` | **Mandatory** directory structure |
| Language | `.claude/rules/idioma.md` | Language setting + Spanish accents |
| Security | `.claude/rules/seguridad.md` | OWASP, secrets, inputs |
| Quality | `.claude/rules/calidad.md` | Conventions, testing, docs |

## Skills (51 total)

### Development & Quality

| Skill | Triggers (es) | Triggers (en) | Description |
|-------|--------------|---------------|-------------|
| `/prompt-contract` | "contrato", "prompt contract" | "contract", "prompt contract" | Structured contract before implementing |
| `/agent-review` | "auto-revisión", "revisar esto" | "review this", "self-review", "agent review" | Sub-agents review code post-implementation |
| `/reverse-prompt` | (auto on implementation) | (auto on implementation) | 5+ clarifying questions before implementing |
| `/stochastic-multi-agent-consensus` | "consenso", "N agentes" | "consensus", "poll agents" | N agents analyze, aggregate by consensus |
| `/model-chat` | "debate de modelos" | "model chat", "agent debate" | 5+ instances debate in shared room |
| `/revisar-codigo-gemini` | "revisar código con Gemini" | "review code with Gemini" | Code review with Gemini as reviewer |
| `/consultar-gemini` | "consultar a Gemini" | "ask Gemini" | Architecture/decision consultations with Gemini |
| `/generar-informe` | "informe de estado" | "status report", "generate report" | Structured project status report |
| `/corregir-ortografia` | "corregir acentos" | "fix spelling", "fix accents" | Fix accents in markdown files |

### Research

| Skill | Triggers | Description |
|-------|----------|-------------|
| `/revision-sistematica` | "búsqueda sistemática", "systematic review" | 7 phases, 6 APIs, PRISMA-S methodology |
| `/x-search` | "buscar en X/Twitter", "search X/Twitter" | Search with Grok API |
| `/consultar-conocimiento` | "consultar conocimiento", "query knowledge" | Query docs/knowledge/ |

### Design & Diagrams

| Skill | Triggers | Description |
|-------|----------|-------------|
| `/design-website` | "diseñar sitio web", "design website" | Premium HTML mockups |
| `/diagram-generator` | "generar diagrama", "generate diagram" | Hand-drawn style diagrams |
| `/excalidraw-flowchart` | "flowchart", "diagrama de flujo" | Excalidraw flowcharts |

### Automation & Leads

| Skill | Description |
|-------|-------------|
| `/gmail` | Gmail operations (search, read, send, reply, label, archive) |
| `/gmail-inbox` | Multi-account Gmail inbox management |
| `/gmail-label` | Auto-label Gmail emails |
| `/inbox-cleaner` | Clean inbox (important vs noise) |
| `/gmaps-leads` | B2B leads from Google Maps |
| `/scrape-leads` | Apify scraping + iterative filter refinement |
| `/classify-leads` | LLM-based lead classification |
| `/casualize-names` | Convert formal names to casual versions |
| `/instantly-campaigns` | Cold email campaigns with A/B testing |
| `/instantly-autoreply` | Auto-generate smart replies to Instantly threads |
| `/linkedin-response` | Respond to LinkedIn messages with human-like style |
| `/create-proposal` | Create PandaDoc proposals + follow-up emails |
| `/upwork-apply` | Upwork job scraping + personalized proposals |
| `/onboarding-kickoff` | Post-kickoff automation (leads, campaigns, auto-reply) |
| `/welcome-email` | Send welcome email sequence to new clients |

### YouTube

| Skill | Description |
|-------|-------------|
| `/youtube-outliers` | Detect viral videos in your niche |
| `/youtube-tracker` | Autonomous daily competitor tracker (GitHub Actions) |
| `/youtube-channel-analysis` | Complete channel analysis (Trends + competitors + Studio) |
| `/video-edit` | Edit talking-head videos (VAD silence removal + 3D transitions) |
| `/video-to-action` | Extract actionable steps from YouTube videos using Gemini |
| `/course-slideshow` | Generate Excalidraw slideshows from course outlines |
| `/outline-generator` | Generate structured course outlines |
| `/thumbnail-generator` | Generate YouTube thumbnails with face-swap + style variations |
| `/recreate-thumbnails` | Face-swap in existing YouTube thumbnails |
| `/title-variants` | Generate title variants for YouTube videos |
| `/internationalize-metadata` | Internationalize YouTube metadata for dubbed versions |
| `/cross-niche-outliers` | Find viral videos in adjacent niches (TubeLab) |
| `/pan-3d-transition` | Create 3D swivel transition effects for videos (Remotion) |

### Infrastructure

| Skill | Description |
|-------|-------------|
| `/browser-stealth` | Bot detection bypass with puppeteer-extra-plugin-stealth |
| `/multi-agent-chrome` | Orchestrate parallel browser automation with multiple Chrome |
| `/modal-deploy` | Deploy workflows to Modal as persistent HTTP endpoints |
| `/add-webhook` | Create Modal webhooks for event-based execution |
| `/local-server` | Run Claude orchestrator locally with Cloudflare tunneling |

### Miscellaneous

| Skill | Description |
|-------|-------------|
| `/algorithmic-art` | Create algorithmic art with p5.js and seeded randomness |
| `/wework-booking` | Book WeWork slots for the next 30 days automatically |
| `/generate-report` | Generate weather reports (Open-Meteo API) |

## Knowledge Management

- **Research & findings**: `docs/knowledge/` with index at `INDEX.md`
- **Categories**: competitors, market, methodology, regulations, technology
- **Query**: `/consultar-conocimiento [topic]` or `/query-knowledge [topic]`
- **Always update INDEX.md** when adding or removing documents

## Plans

- **Actionable plans**: `docs/plans/` with checkboxes
- **Master plans**: `plan-maestro-*.md` with sub-plan references
- **Track progress**: update checkboxes as tasks complete

## MCPs

Not installed by default. See `setup/mcp-catalog.md` for the complete catalog.
To activate: copy `.mcp.json.example` to `.mcp.json` and uncomment servers.
