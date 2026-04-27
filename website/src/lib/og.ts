import { SITE_URL } from './constants';

interface OgParams {
  title: string;
  description: string;
  label?: string;
  tags?: string[];
}

export function buildOgUrl({ title, description, label, tags }: OgParams): string {
  const params = new URLSearchParams({ title, description });
  if (label) params.set('label', label);
  if (tags?.length) params.set('tags', tags.join(','));
  return `${SITE_URL}/og?${params.toString()}`;
}
