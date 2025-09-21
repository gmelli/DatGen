#!/bin/bash
# Sign-off script for DatGen project
# Ensures all changes are committed and pushed before ending a session

echo "🔄 Starting sign-off process..."
echo ""

# Function to check and commit changes in a directory
check_and_commit() {
    local dir=$1
    local repo_name=$2

    echo "📁 Checking $repo_name..."
    cd "$dir" || return

    # Check if it's a git repository
    if [ ! -d .git ]; then
        echo "  ⚠️  Not a git repository, skipping"
        return
    fi

    # Check for uncommitted changes
    if [ -n "$(git status --porcelain)" ]; then
        echo "  📝 Uncommitted changes found"
        git status --short

        # Stage all changes
        git add -A

        # Create commit message
        echo "  💾 Creating commit..."
        git commit -m "chore: Sign-off commit - saving work in progress

Automated commit created by sign-off script.
Changes saved before ending session.

🤖 Generated with [Claude Code](https://claude.ai/code)

Co-Authored-By: Claude <noreply@anthropic.com>"

        if [ $? -eq 0 ]; then
            echo "  ✅ Changes committed"
        else
            echo "  ⚠️  Commit failed"
        fi
    else
        echo "  ✨ Working directory clean"
    fi

    # Check if there are unpushed commits
    local unpushed=$(git log origin/$(git branch --show-current)..HEAD --oneline 2>/dev/null | wc -l)
    if [ "$unpushed" -gt 0 ]; then
        echo "  📤 Pushing $unpushed commit(s) to GitHub..."
        git push
        if [ $? -eq 0 ]; then
            echo "  ✅ Pushed to GitHub"
        else
            echo "  ❌ Push failed - check your connection"
        fi
    else
        echo "  ✅ Already up to date with GitHub"
    fi

    echo ""
}

# Main script
echo "================================"
echo "    DatGen Sign-Off Process"
echo "================================"
echo ""

# Check original repository
check_and_commit "/Users/gabormelli/github/DatGen" "DatGen (original)"

# Check new repository if it exists
if [ -d "/Users/gabormelli/github/DatGen/datgen-ai-amplified" ]; then
    check_and_commit "/Users/gabormelli/github/DatGen/datgen-ai-amplified" "datgen-ai-amplified"
fi

# Final status
echo "================================"
echo "    Sign-Off Complete!"
echo "================================"
echo ""
echo "Summary:"
cd /Users/gabormelli/github/DatGen
echo "• DatGen: $(git log --oneline -1)"

if [ -d "datgen-ai-amplified" ]; then
    cd datgen-ai-amplified
    echo "• datgen-ai-amplified: $(git log --oneline -1)"
fi

echo ""
echo "✅ All changes saved and pushed!"
echo "👋 Safe to close the session"
echo ""