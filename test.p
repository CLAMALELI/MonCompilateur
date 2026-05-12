VAR
  i, j, n, factoriel : INTEGER;
  x, y : DOUBLE;
  c : CHAR;
  estGrand, estPair : BOOLEAN.

BEGIN
  i := 1;
  n := 5;
  factoriel := 1;

  FOR i := 1 TO n DO
    factoriel := factoriel * i;
  DISPLAY factoriel;

  i := 0;
  WHILE i < 10 DO
  BEGIN
    estPair := (i % 2) == 0;
    IF estPair THEN
      DISPLAY i
    ELSE
      i := i + 0
    ;
    i := i + 1
  END;

  x := 3.14;
  y := 2.0;
  DISPLAY x;
  DISPLAY x + y;
  DISPLAY x * y;

  j := 10;
  estGrand := j >= 5;
  DISPLAY estGrand;

  i := 3;
  j := 5;
  DISPLAY i + j;
  DISPLAY i * j;
  DISPLAY j - i;
  DISPLAY j / i;
  DISPLAY j % i;

  i := 6;
  j := 3;
  DISPLAY i && j;
  DISPLAY i || j;

  i := 1;
  IF i == 1 THEN
    DISPLAY i
  ELSE
    DISPLAY j

END.