@ECHO ON

@rem generate Markdown to HTML
call gitbook build

@rem run server
call gitbook serve
