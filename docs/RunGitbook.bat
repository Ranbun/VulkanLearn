
@ECHo OFF
SET arg1=%1
@ECHO ON

@rem generate Markdown to HTML
call gitbook build

@rem run server
if (%arg1%) == () (
    call gitbook serve
)
else (
    call gitbook serve --port %arg1%
)
