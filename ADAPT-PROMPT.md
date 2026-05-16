# Adapt Your Project to the Machote Structure

> Copy everything below the line into Claude Code, opened at the root of the project you want to adapt.

---

I need you to adapt this project to follow the "Machote" Claude Code organization standard. This is a team-wide standard for how Claude projects should be structured. Here's exactly what to do:

## Step 1 — Clone the template (temporary)

```bash
git clone https://github.com/jesuscottage/machote.git /tmp/machote-template
```

## Step 2 — Read the template's structure

Read these files from the cloned template to understand the target organization:
- `/tmp/machote-template/CLAUDE.md`
- `/tmp/machote-template/.claude/rules/organizacion.md`
- `/tmp/machote-template/README.md` (the "Adapt to an existing project" section)

## Step 3 — Audit this project (READ-ONLY)

Before making ANY changes, audit the current state:
1. Check if `.claude/` already exists and what's inside it
2. Check if there are skills, rules, or agents anywhere outside `.claude/` (e.g., in a `skills/` folder at root, or scattered in other locations)
3. Check if `docs/knowledge/`, `docs/plans/`, `scripts/`, or `setup/` already exist
4. Check if `CLAUDE.md` already exists
5. Check if `.claude/settings.json` already exists
6. Check if `.mcp.json` already exists
7. List any potential conflicts

**Present me a summary of what exists vs. what will be created, and wait for my confirmation before proceeding.**

## Step 4 — Apply the structure (ADDITIVE ONLY)

After I confirm, apply the machote structure following these strict safety rules:

### What you MUST do:
- **Create** `.claude/rules/` and copy ALL 4 rule files from the template (organizacion.md, idioma.md, seguridad.md, calidad.md)
- **Create** `.claude/agents/` and copy `investigador.md` from the template
- **Create** `.claude/commands/` and copy `commit.md` from the template
- **Create** `.claude/context/` and copy `reminders.md` from the template (then customize it for this project)
- **Create** `.claude/skills/` and copy ALL skill folders from the template
- **Create** `docs/knowledge/INDEX.md` from the template
- **Create** `docs/plans/` directory
- **Create** `scripts/` and copy `fix-spanish-ortho.sh` from the template
- **Create** `setup/` and copy `mcp-catalog.md` from the template
- **Create** `.mcp.json.example` from the template
- **Create** `.gitignore` entries — MERGE with existing .gitignore (don't replace)

### What you MUST NOT do:
- **Never modify** any existing source code files
- **Never modify** package.json, requirements.txt, Docker files, CI/CD configs, or any build configuration
- **Never modify** .env files or any credentials/secrets
- **Never modify** any API connections, endpoints, or service configurations
- **Never delete** any existing files or directories
- **Never move** existing files — if skills exist elsewhere, COPY them into `.claude/skills/` and tell me where the originals are so I can decide whether to remove them

### Merge behavior:
- If `CLAUDE.md` exists: see Step 5 below (full integration, not just appending)
- If `.gitignore` exists: **append** missing entries from the template — don't replace

## Step 5 — Integrate CLAUDE.md (CRITICAL)

This is not a simple append — the goal is to produce the **best possible CLAUDE.md** for this project by integrating the existing content with the machote standard.

If CLAUDE.md already exists:
1. **Read the existing CLAUDE.md thoroughly** — understand what project-specific information it contains (stack, structure, conventions, known issues, debugging tips, etc.)
2. **Read the machote's CLAUDE.md** as the target structure reference
3. **Write a new integrated CLAUDE.md** that:
   - Keeps ALL project-specific information from the original (stack, structure, conventions, business rules, known issues, etc.)
   - Reorganizes it into the machote's clean structure (sections: Project Structure, Rules, Skills, Knowledge Management, Plans, MCPs)
   - Adds the complete skills table from the machote (with bilingual triggers)
   - Adds references to the new rules, knowledge base, plans directory, and setup files
   - Removes any redundancy or disorganization from the original
   - Results in a single, cohesive document — not "old content + new content glued together"
4. **Before overwriting**, show me a diff or summary of what changed and wait for confirmation

If CLAUDE.md does not exist:
- Create it from the machote template and customize it for this project's stack, structure, and context

## Step 6 — Customize for this project

After copying the structure:
1. Update `.claude/context/reminders.md` with this project's actual structure, stack, and key information
2. If the project has business rules, create `.claude/rules/negocio.md` with the project's specific invariants
3. If the project uses a specific language setting, update `.claude/rules/idioma.md` (`idioma-principal: en` or `es`)
4. If the project already had rules, agents, or context files, integrate their content into the new structure (don't lose any project-specific instructions)

## Step 7 — Clean up

```bash
rm -rf /tmp/machote-template
```

## Step 8 — Summary

Show me:
1. What was created (list of new files/directories)
2. What was merged (.gitignore, CLAUDE.md)
3. What was NOT touched (existing project files)
4. Any skills or configs that existed outside `.claude/` that I should review for migration
5. Recommended next steps (e.g., activate MCPs)

---

**Remember: this is an ADDITIVE operation. If in doubt about whether something should be modified, DON'T modify it — ask me first.**
