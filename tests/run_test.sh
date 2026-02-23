#!/bin/bash

# Se placer dans le dossier du script pour que les chemins relatifs soient corrects
cd "$(dirname "$0")"

# Couleurs pour l'affichage
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Compteurs
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Chemin vers l'exécutable (depuis tests/ : ../webserv)
PARSER_BIN="../webserv"

# Dossier des tests
TEST_DIR="./config_files"

echo "========================================="
echo "   WEBSERV CONFIG PARSER - TEST SUITE   "
echo "========================================="
echo ""

# Fonction pour tester un fichier valide
test_valid() {
    local file=$1
    local filename=$(basename "$file")

    TOTAL_TESTS=$((TOTAL_TESTS + 1))

    echo -n "Testing VALID: $filename ... "

    # Exécuter le parser et capturer la sortie
    if $PARSER_BIN "$file" > /dev/null 2>&1; then
        echo -e "${GREEN}✓ PASS${NC}"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo -e "${RED}✗ FAIL${NC} (should have passed)"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        echo "  Error output:"
        $PARSER_BIN "$file" 2>&1 | sed 's/^/    /'
    fi
}

# Fonction pour tester un fichier invalide
test_invalid() {
    local file=$1
    local filename=$(basename "$file")

    TOTAL_TESTS=$((TOTAL_TESTS + 1))

    echo -n "Testing INVALID: $filename ... "

    # Exécuter le parser et vérifier qu'il échoue
    if $PARSER_BIN "$file" > /dev/null 2>&1; then
        echo -e "${RED}✗ FAIL${NC} (should have failed)"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    else
        echo -e "${GREEN}✓ PASS${NC}"
        PASSED_TESTS=$((PASSED_TESTS + 1))
        # Afficher l'erreur détectée
        error_msg=$($PARSER_BIN "$file" 2>&1 | grep "(PARSER)" | head -n 1)
        if [ -n "$error_msg" ]; then
            echo -e "  ${YELLOW}→${NC} $error_msg"
        fi
    fi
}

# Vérifier que l'exécutable existe
if [ ! -f "$PARSER_BIN" ]; then
    echo -e "${RED}Error: Parser executable not found at $PARSER_BIN${NC}"
    echo "Please compile your project first or update PARSER_BIN variable"
    exit 1
fi

# Vérifier que le dossier de tests existe
if [ ! -d "$TEST_DIR" ]; then
    echo -e "${RED}Error: Test directory not found at $TEST_DIR${NC}"
    exit 1
fi

echo "=== VALID CONFIGURATION TESTS ==="
echo ""

# Tester tous les fichiers valides
for file in $TEST_DIR/valid_*.conf; do
    if [ -f "$file" ]; then
        test_valid "$file"
    fi
done

echo ""
echo "=== INVALID CONFIGURATION TESTS ==="
echo ""

# Tester tous les fichiers invalides
for file in $TEST_DIR/invalid_*.conf; do
    if [ -f "$file" ]; then
        test_invalid "$file"
    fi
done

# Résumé
echo ""
echo "========================================="
echo "              TEST SUMMARY               "
echo "========================================="
echo "Total tests:  $TOTAL_TESTS"
echo -e "Passed:       ${GREEN}$PASSED_TESTS${NC}"
echo -e "Failed:       ${RED}$FAILED_TESTS${NC}"
echo ""

# Code de sortie
if [ $FAILED_TESTS -eq 0 ]; then
    echo -e "${GREEN}All tests passed! ✓${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed ✗${NC}"
    exit 1
fi
