; http://www.emacswiki.org/emacs/ModeTutorial?vm=r
(defvar un-language-mode-hook nil)
(defvar un-language-mode-map
  (let ((map (make-sparse-keymap)))
    (define-key map "\C-j" 'newline-and-indent)
    map)
  "Keymap for Un major mode")
;;;###autoload
(add-to-list 'auto-mode-alist '("\\.b\\'" . un-language-mode))
(defconst un-language-font-lock-keywords-1
  (list
   '("\\<\\(auto\\|break\\|case\\|class\\|continue\\|default\\|do\\|else\\|for\\|if\\|length\\|new\\|return\\|switch\\|void\\|while\\)\\>" . font-lock-keyword-face)
   '("\\('\\w*'\\)" . font-lock-variable-name-face))
  "Highlighting for Un mode")
(defconst un-language-font-lock-keywords-2
  (append un-language-font-lock-keywords-1
          (list
           '("\\<\\(true\\|false\\)\\>" . font-lock-constant-face)
           '("\\<\\(Bool\\|Byte\\|Double\\|Float\\|Int\\|\\Long\\)\\>" . font-lock-builtin-face)))
  "Additional keywords to highlight in Un mode")
(defvar un-language-font-lock-keywords un-language-font-lock-keywords-2
  "Default highlighting expressions for Un mode")
(defun un-language-indent-line ()
  "Indent current line as Un code"
  (interactive)
  (beginning-of-line)
  (if (bobp)
      (indent-line-to 0)
    (let (cur-indent)
      (save-excursion
        (forward-line -1)
        (setq cur-indent (current-indentation))
        (if (looking-at "^.*\{[^\}]*$")
            (progn
              (setq cur-indent (+ cur-indent 4))
              (setq not-indented nil))))
      (indent-line-to cur-indent))))
(defvar un-language-mode-syntax-table
  (let ((st (make-syntax-table)))
    (modify-syntax-entry ?/ ". 124b" st)
    (modify-syntax-entry ?* ". 23" st)
    (modify-syntax-entry ?\n "> b" st)
    st)
  "Syntax table for un-language-mode")
(defun un-language-mode ()
  "Major mode for editing Un langage files"
  (interactive)
  (kill-all-local-variables)
  (set-syntax-table un-language-mode-syntax-table)
  (use-local-map un-language-mode-map)
  (set (make-local-variable 'font-lock-defaults) '(un-language-font-lock-keywords))
  (set (make-local-variable 'indent-line-function) 'un-language-indent-line)
  (setq major-mode 'un-language-mode)
  (setq mode-name "Un")
  (run-hooks 'un-language-mode-hook))
(provide 'un-language-mode)
