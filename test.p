VAR
  x : INTEGER;
  y : INTEGER;
  z : INTEGER;
  i : INTEGER;
  j : INTEGER;
  r : BOOLEAN;
  c : CHAR;
  d : DOUBLE;
  e : DOUBLE;
  s : STRING;
  t : STRING.
BEGIN
  x := 10;
  y := 3;
  z := x + y;
  DISPLAY z;
  z := x - y;
  DISPLAY z;
  z := x * y;
  DISPLAY z;
  z := x / y;
  DISPLAY z;
  z := x % y;
  DISPLAY z;
  x := 5;
  y := 10;
  IF x == y THEN
    DISPLAY x
  ELSE
    DISPLAY y;
  IF x != y THEN
    DISPLAY x;
  IF x < y THEN
    DISPLAY x;
  IF y > x THEN
    DISPLAY y;
  IF x <= y THEN
    DISPLAY x;
  IF y >= x THEN
    DISPLAY y;
  i := 0;
  WHILE i < 5 DO
    BEGIN
      DISPLAY i;
      i := i + 1
    END;
  FOR i := 1 TO 5 DO
    DISPLAY i;
  FOR i := 5 DOWNTO 1 DO
    DISPLAY i;
  i := 0;
  REPEAT
    DISPLAY i;
    i := i + 1
  UNTIL i == 5;
  i := 0;
  DO
    BEGIN
      DISPLAY i;
      i := i + 1
    END
  WHILE i < 5;
  x := 2;
  CASE x OF
    1 : DISPLAY x;
    2 : DISPLAY x;
    3 : DISPLAY x
  END;
  x := 5;
  y := 10;
  r := x < y;
  DISPLAY r;
  r := x > y;
  DISPLAY r;
  r := x == y;
  DISPLAY r;
  c := 'A';
  DISPLAY c;
  c := 'z';
  DISPLAY c;
  d := 3.14;
  e := 2.0;
  DISPLAY d;
  d := d + e;
  DISPLAY d;
  d := d - e;
  DISPLAY d;
  d := d * e;
  DISPLAY d;
  d := d / e;
  DISPLAY d;
  s := "Hello";
  t := " World";
  DISPLAY s;
  s := s + t;
  DISPLAY s;
  x := 6;
  y := 3;
  z := x && y;
  DISPLAY z;
  z := x || y;
  DISPLAY z;
  i := 1;
  WHILE i <= 3 DO
    BEGIN
      j := 1;
      WHILE j <= 3 DO
        BEGIN
          DISPLAY j;
          j := j + 1
        END;
      i := i + 1
    END
END.