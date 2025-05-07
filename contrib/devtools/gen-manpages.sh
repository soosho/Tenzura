#!/bin/sh

TOPDIR=${TOPDIR:-$(git rev-parse --show-toplevel)}
SRCDIR=${SRCDIR:-$TOPDIR/src}
MANDIR=${MANDIR:-$TOPDIR/doc/man}

TENZURAD=${TENZURAD:-$SRCDIR/tenzurad}
TENZURACLI=${TENZURACLI:-$SRCDIR/tenzura-cli}
TENZURATX=${TENZURATX:-$SRCDIR/tenzura-tx}
TENZURAQT=${TENZURAQT:-$SRCDIR/qt/tenzura-qt}

[ ! -x $TENZURAD ] && echo "$TENZURAD not found or not executable." && exit 1

# The autodetected version git tag can screw up manpage output a little bit
TENZVER=($($TENZURACLI --version | head -n1 | awk -F'[ -]' '{ print $6, $7 }'))

# Create a footer file with copyright content.
# This gets autodetected fine for tenzurad if --version-string is not set,
# but has different outcomes for tenzura-qt and tenzura-cli.
echo "[COPYRIGHT]" > footer.h2m
$TENZURAD --version | sed -n '1!p' >> footer.h2m

for cmd in $TENZURAD $TENZURACLI $TENZURATX $TENZURAQT; do
  cmdname="${cmd##*/}"
  help2man -N --version-string=${TENZVER[0]} --include=footer.h2m -o ${MANDIR}/${cmdname}.1 ${cmd}
  sed -i "s/\\\-${TENZVER[1]}//g" ${MANDIR}/${cmdname}.1
done

rm -f footer.h2m
